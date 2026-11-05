// Copyright 2026 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

// Lecture en temps réel de la couleur des 58 LED par raw HID (commande 0xC0), pour le configurateur web.
// Lecture seule : aucune écriture EEPROM, aucun changement d'état, pas de déverrouillage Vial requis.
// Spécification : docs/led-stream-protocol.md
//
// Source des couleurs : le tampon du driver WS2812 (ws2812_leds[], global dans le driver vendor RP2040).
// Il contient la valeur réellement envoyée aux LED, après effet, ledmaps et luminosité. Chaque moitié ne
// remplit que ses 29 LED (indices locaux 0-28) : les LED de l'autre moitié sont lues par un RPC split.

#include "quantum.h"
#include "raw_hid.h"
#include "via.h"
#include "ws2812.h"
#include "transactions.h"
#include "split_util.h"

#define LED_STREAM_CMD 0xC0
#define LED_STREAM_PROTOCOL_VERSION 1
#define LED_STREAM_REQ_MARKER 0xFF  // octet 2 des requêtes ; jamais un statut valide (voir la spec)

#define LED_STREAM_COLORS_PER_PACKET 9      // 5 + 9 × 3 = 32 octets
#define LED_STREAM_POSITIONS_PER_PACKET 13  // 5 + 13 × 2 = 31 octets

#define LED_STREAM_LEFT_COUNT 29  // = rgb_matrix.split_count[0] de keyboard.json

enum led_stream_sub {
    LS_INFO      = 0x00,
    LS_POSITIONS = 0x01,
    LS_COLORS    = 0x02,
};

enum led_stream_status {
    LS_OK                 = 0x00,
    LS_ERR_UNKNOWN_SUB    = 0x01,
    LS_ERR_OUT_OF_RANGE   = 0x02,
    LS_ERR_BAD_REQUEST    = 0x03,
    LS_REMOTE_UNAVAILABLE = 0x10,  // lien split coupé ou RPC en échec : LED de l'autre moitié à 0
    LS_REMOTE_MISMATCH    = 0x11,  // autre moitié sans ce firmware (pas reflashée) : LED à 0
};

enum led_stream_flags {
    LS_FLAG_SPLIT_RPC        = 1 << 0,
    LS_FLAG_REMOTE_CONNECTED = 1 << 1,
    LS_FLAG_RGB_ENABLED      = 1 << 2,
};

extern ws2812_led_t ws2812_leds[];

_Static_assert(RGB_MATRIX_LED_COUNT == 2 * LED_STREAM_LEFT_COUNT, "split_count de keyboard.json modifié");

static bool led_is_left(uint8_t led) {
    return led < LED_STREAM_LEFT_COUNT;
}

static uint8_t led_local_index(uint8_t led) {
    return led_is_left(led) ? led : led - LED_STREAM_LEFT_COUNT;
}

static void copy_local_rgb(uint8_t local, uint8_t count, uint8_t *out) {
    for (uint8_t i = 0; i < count; i++) {
        out[3 * i + 0] = ws2812_leds[local + i].r;
        out[3 * i + 1] = ws2812_leds[local + i].g;
        out[3 * i + 2] = ws2812_leds[local + i].b;
    }
}

// --- RPC split (exécuté sur l'esclave) -------------------------------------------------------------------

typedef struct PACKED {
    uint8_t start;  // indice local dans la moitié (0-28)
    uint8_t count;
} led_rpc_req_t;

typedef struct PACKED {
    uint8_t version;  // LED_STREAM_PROTOCOL_VERSION : détecte une moitié restée sur un ancien firmware
    uint8_t rgb[LED_STREAM_COLORS_PER_PACKET * 3];
} led_rpc_resp_t;

_Static_assert(sizeof(led_rpc_resp_t) <= RPC_S2M_BUFFER_SIZE, "réponse RPC trop grande");

static void led_stream_slave_rpc(uint8_t in_len, const void *in, uint8_t out_len, void *out) {
    const led_rpc_req_t *req  = in;
    led_rpc_resp_t      *resp = out;

    if (in_len != sizeof(*req) || out_len != sizeof(*resp)) {
        return;
    }
    memset(resp, 0, sizeof(*resp));
    if (req->count > LED_STREAM_COLORS_PER_PACKET || req->start + req->count > LED_STREAM_LEFT_COUNT) {
        return;  // version 0 → le maître signale LS_REMOTE_MISMATCH
    }
    resp->version = LED_STREAM_PROTOCOL_VERSION;
    copy_local_rgb(req->start, req->count, resp->rgb);
}

void led_stream_init(void) {
    transaction_register_rpc(RPC_ID_KB_LED_STREAM, led_stream_slave_rpc);
}

// --- Commandes raw HID (exécutées sur le maître) ---------------------------------------------------------

static bool matrix_position(uint8_t led, uint8_t *row, uint8_t *col) {
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        for (uint8_t c = 0; c < MATRIX_COLS; c++) {
            if (g_led_config.matrix_co[r][c] == led) {
                *row = r;
                *col = c;
                return true;
            }
        }
    }
    *row = *col = 0xFF;
    return false;
}

static uint8_t handle_info(uint8_t *out) {
    out[0] = LED_STREAM_PROTOCOL_VERSION;
    out[1] = RGB_MATRIX_LED_COUNT;
    out[2] = LED_STREAM_COLORS_PER_PACKET;
    out[3] = LED_STREAM_POSITIONS_PER_PACKET;
    out[4] = LED_STREAM_LEFT_COUNT;
    out[5] = is_keyboard_left() ? 0 : 1;
    out[6] = LS_FLAG_SPLIT_RPC | (is_transport_connected() ? LS_FLAG_REMOTE_CONNECTED : 0) | (rgb_matrix_is_enabled() ? LS_FLAG_RGB_ENABLED : 0);
    out[7] = RGB_MATRIX_MAXIMUM_BRIGHTNESS;
    out[8] = rgb_matrix_get_val();
    out[9] = MATRIX_ROWS;
    out[10] = MATRIX_COLS;
    return LS_OK;
}

static uint8_t handle_positions(uint8_t start, uint8_t *count, uint8_t *out) {
    *count = MIN(LED_STREAM_POSITIONS_PER_PACKET, RGB_MATRIX_LED_COUNT - start);
    for (uint8_t i = 0; i < *count; i++) {
        matrix_position(start + i, &out[2 * i], &out[2 * i + 1]);
    }
    return LS_OK;
}

static uint8_t handle_colors(uint8_t start, uint8_t *count, uint8_t *out) {
    uint8_t status = LS_OK;
    bool    master_left = is_keyboard_left();

    *count = MIN(LED_STREAM_COLORS_PER_PACKET, RGB_MATRIX_LED_COUNT - start);
    memset(out, 0, *count * 3);

    // Un paquet peut chevaucher la frontière 28/29 : découpé en deux plages, une par moitié.
    uint8_t end = start + *count;
    for (uint8_t first = start; first < end;) {
        uint8_t last = led_is_left(first) ? MIN(end, LED_STREAM_LEFT_COUNT) : end;
        uint8_t n    = last - first;
        uint8_t *dst = out + 3 * (first - start);

        if (led_is_left(first) == master_left) {
            copy_local_rgb(led_local_index(first), n, dst);
        } else {
            led_rpc_req_t  req  = {.start = led_local_index(first), .count = n};
            led_rpc_resp_t resp = {0};
            if (!transaction_rpc_exec(RPC_ID_KB_LED_STREAM, sizeof(req), &req, sizeof(resp), &resp)) {
                status = LS_REMOTE_UNAVAILABLE;
            } else if (resp.version != LED_STREAM_PROTOCOL_VERSION) {
                status = LS_REMOTE_MISMATCH;
            } else {
                memcpy(dst, resp.rgb, n * 3);
            }
        }
        first = last;
    }
    return status;
}

// Requête : [0xC0, sous-commande, 0xFF, start, 0…]
// Réponse : [0xC0, sous-commande, statut, start, count, données…]
void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if (data[0] != LED_STREAM_CMD) {
        data[0] = id_unhandled;  // comportement par défaut de via.c (appelé aussi pour des sous-ids VIA inconnus)
        return;
    }

    uint8_t sub   = data[1];
    uint8_t start = data[3];
    uint8_t count = 0;
    uint8_t status;

    if (length < 32 || data[2] != LED_STREAM_REQ_MARKER) {
        status = LS_ERR_BAD_REQUEST;
        memset(&data[3], 0, length - 3);
    } else if (sub == LS_INFO) {
        memset(&data[3], 0, length - 3);
        status = handle_info(&data[3]);
    } else if (sub != LS_POSITIONS && sub != LS_COLORS) {
        status = LS_ERR_UNKNOWN_SUB;
        memset(&data[3], 0, length - 3);
    } else if (start >= RGB_MATRIX_LED_COUNT) {
        status = LS_ERR_OUT_OF_RANGE;
        memset(&data[4], 0, length - 4);
    } else {
        memset(&data[4], 0, length - 4);
        status = sub == LS_POSITIONS ? handle_positions(start, &count, &data[5]) : handle_colors(start, &count, &data[5]);
        data[4] = count;
    }
    data[2] = status;
}
