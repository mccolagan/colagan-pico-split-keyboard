// Copyright 2026 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

// Couleurs et effet par couche, modifiables depuis le configurateur (commande 0xC0, voir led_stream.c) et
// enregistrés dans le bloc clavier de l'EEPROM. Valeurs par défaut : tableau ledmaps de la keymap.
//
// Split : le maître (moitié branchée en USB) est la source de vérité. Il pousse à l'esclave les couleurs de
// sa moitié par RPC, à chaque écriture et en entier quand l'esclave n'est pas à jour (identifiant de
// synchronisation vérifié toutes les 500 ms). L'effet est appliqué par le maître seul : RGB_MATRIX_SPLIT
// transmet le mode, la couleur et la vitesse à l'esclave.

#include "quantum.h"
#include "eeconfig.h"
#include "transactions.h"
#include "split_util.h"
#include "vialrgb.h"
#include "led_layers.h"

_Static_assert(LED_LAYERS == DYNAMIC_KEYMAP_LAYER_COUNT, "une entrée de couleurs par couche Vial");
_Static_assert(LED_LAYERS_LEDS == RGB_MATRIX_LED_COUNT, "nombre de LED");

#define LEFT_COUNT 29
#define PREVIEW_MS 3000        // côté maître : sans renouvellement par le configurateur
#define SLAVE_PREVIEW_MS 1500  // côté esclave : sans ping du maître
#define PING_MS 500

typedef struct __attribute__((packed)) {
    uint8_t            rgb[LED_LAYERS][LED_LAYERS_LEDS][3];
    uint8_t            transparent[LED_LAYERS][(LED_LAYERS_LEDS + 7) / 8];
    led_layer_effect_t effect[LED_LAYERS];
} led_layers_data_t;

_Static_assert(sizeof(led_layers_data_t) == EECONFIG_KB_DATA_SIZE, "EECONFIG_KB_DATA_SIZE (config.h) à ajuster");

static led_layers_data_t data;
static uint8_t           preview      = LED_LAYERS_NO_PREVIEW;
static uint32_t          preview_time = 0;

// --- Stockage --------------------------------------------------------------------------------------------

static bool is_transparent(uint8_t layer, uint8_t led) {
    return data.transparent[layer][led / 8] & (1 << (led % 8));
}

static void set_transparent(uint8_t layer, uint8_t led, bool on) {
    if (on) {
        data.transparent[layer][led / 8] |= 1 << (led % 8);
    } else {
        data.transparent[layer][led / 8] &= ~(1 << (led % 8));
    }
}

static void save(const void *field, size_t size) {
    eeconfig_update_kb_datablock(field, (const uint8_t *)field - (const uint8_t *)&data, size);
}

static void load_defaults(uint8_t layer) {
    memset(data.rgb[layer], 0, sizeof(data.rgb[layer]));
    memset(data.transparent[layer], 0xFF, sizeof(data.transparent[layer]));
    memset(&data.effect[layer], 0, sizeof(data.effect[layer]));
    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        for (uint8_t col = 0; col < MATRIX_COLS; col++) {
            uint8_t led = g_led_config.matrix_co[row][col];
            if (led != NO_LED && led_layers_default_color(layer, row, col, data.rgb[layer][led])) {
                set_transparent(layer, led, false);
            }
        }
    }
}

// Appelé par QMK quand l'EEPROM est réinitialisée (bootmagic, EEPROM vierge)
void eeconfig_init_kb_datablock(void) {
    for (uint8_t layer = 0; layer < LED_LAYERS; layer++) {
        load_defaults(layer);
    }
    eeconfig_update_kb_datablock(&data, 0, sizeof(data));
}

__attribute__((weak)) bool led_layers_default_color(uint8_t layer, uint8_t row, uint8_t col, uint8_t rgb[3]) {
    return false;
}

// --- Split : RPC exécutés sur l'esclave ------------------------------------------------------------------

enum { RPC_WRITE = 1, RPC_PING = 2, RPC_SET_SYNC = 3 };

typedef struct __attribute__((packed)) {
    uint8_t op;
    uint8_t layer;
    uint8_t start;  // indice global de LED
    uint8_t count;
    uint8_t mask;
    uint8_t rgb[LED_LAYERS_RANGE_MAX * 3];
} rpc_write_t;

typedef struct __attribute__((packed)) {
    uint8_t op;
    uint8_t sync_id;  // RPC_SET_SYNC
    uint8_t preview;  // RPC_PING
} rpc_ctrl_t;

_Static_assert(sizeof(rpc_write_t) <= RPC_M2S_BUFFER_SIZE, "RPC trop grand");

static uint8_t  slave_sync_id = 0;  // 0 = jamais synchronisé
static uint32_t slave_ping_time;

static void apply_range(uint8_t layer, uint8_t start, uint8_t count, uint8_t mask, const uint8_t *rgb) {
    for (uint8_t i = 0; i < count; i++) {
        memcpy(data.rgb[layer][start + i], &rgb[3 * i], 3);
        set_transparent(layer, start + i, mask & (1 << i));
    }
}

static void slave_rpc(uint8_t in_len, const void *in, uint8_t out_len, void *out) {
    const uint8_t *op = in;
    if (in_len == 0) {
        return;
    }
    if (*op == RPC_WRITE && in_len == sizeof(rpc_write_t)) {
        const rpc_write_t *w = in;
        if (w->layer < LED_LAYERS && w->count <= LED_LAYERS_RANGE_MAX && w->start + w->count <= LED_LAYERS_LEDS) {
            apply_range(w->layer, w->start, w->count, w->mask, w->rgb);
        }
    } else if (in_len == sizeof(rpc_ctrl_t)) {
        const rpc_ctrl_t *c = in;
        if (c->op == RPC_PING) {
            preview         = c->preview;
            slave_ping_time = timer_read32();
        } else if (c->op == RPC_SET_SYNC) {
            slave_sync_id = c->sync_id;
        }
    }
    if (out_len >= 1) {
        ((uint8_t *)out)[0] = slave_sync_id;
    }
}

// --- Maître : envoi à l'esclave ----------------------------------------------------------------------------

static uint8_t  sync_id = 1;       // identifiant attendu chez l'esclave
static uint8_t  sync_pos = 0xFF;   // prochain bloc à envoyer pendant une synchronisation (0xFF : aucune)
static uint32_t ping_time;

static bool led_on_slave(uint8_t led) {
    return (led < LEFT_COUNT) != is_keyboard_left();
}

static void invalidate_slave(void) {
    sync_id = sync_id == 0xFF ? 1 : sync_id + 1;  // le prochain ping relancera une synchronisation complète
}

static void push_range(uint8_t layer, uint8_t start, uint8_t count) {
    rpc_write_t w = {.op = RPC_WRITE, .layer = layer, .start = start, .count = count};
    for (uint8_t i = 0; i < count; i++) {
        memcpy(&w.rgb[3 * i], data.rgb[layer][start + i], 3);
        if (is_transparent(layer, start + i)) w.mask |= 1 << i;
    }
    if (!transaction_rpc_send(RPC_ID_KB_LED_LAYERS, sizeof(w), &w)) {
        invalidate_slave();
    }
}

// Pousse vers l'esclave la partie de [start, start+count) qui lui appartient
static void push_slave_part(uint8_t layer, uint8_t start, uint8_t count) {
    for (uint8_t i = 0; i < count; i++) {
        uint8_t led = start + i;
        if (led_on_slave(led)) {
            uint8_t n = 1;
            while (i + n < count && led_on_slave(start + i + n)) n++;
            push_range(layer, led, n);
            i += n - 1;
        }
    }
}

static void master_sync_task(void) {
    if (!is_transport_connected()) {
        return;
    }
    if (sync_pos != 0xFF) {
        // un bloc de 8 LED par passage : 4 blocs par couche pour une moitié de 29 LED
        uint8_t first = is_keyboard_left() ? LEFT_COUNT : 0;
        uint8_t layer = sync_pos / 4;
        uint8_t start = first + (sync_pos % 4) * LED_LAYERS_RANGE_MAX;
        uint8_t count = MIN(LED_LAYERS_RANGE_MAX, first + LEFT_COUNT - start);
        push_range(layer, start, count);
        if (++sync_pos == LED_LAYERS * 4) {
            rpc_ctrl_t c = {.op = RPC_SET_SYNC, .sync_id = sync_id};
            transaction_rpc_send(RPC_ID_KB_LED_LAYERS, sizeof(c), &c);
            sync_pos = 0xFF;
        }
        return;
    }
    if (timer_elapsed32(ping_time) >= PING_MS) {
        ping_time      = timer_read32();
        rpc_ctrl_t c   = {.op = RPC_PING, .preview = preview};
        uint8_t    ack = 0;
        if (transaction_rpc_exec(RPC_ID_KB_LED_LAYERS, sizeof(c), &c, sizeof(ack), &ack) && ack != sync_id) {
            sync_pos = 0;
        }
    }
}

// --- Effet par couche (maître) -----------------------------------------------------------------------------

typedef struct {
    bool    enable;
    uint8_t mode;
    hsv_t   hsv;
    uint8_t speed;
} base_effect_t;

#define NO_EFFECT_LAYER 0xFF
static uint8_t       applied_effect = NO_EFFECT_LAYER;  // couche dont l'effet est appliqué
static base_effect_t base;                               // effet global, restauré en sortie de couche

static void apply_effect(const led_layer_effect_t *e) {
    // passe par VialRGB pour la conversion identifiant VialRGB -> mode QMK (fonctions statiques de vialrgb.c)
    uint8_t msg[32] = {0x07, vialrgb_set_mode, e->vialrgb_id & 0xFF, e->vialrgb_id >> 8, e->speed, e->hue, e->sat, e->val};
    vialrgb_set_value(msg, sizeof(msg));
}

static void effect_task(void) {
    uint8_t layer  = led_layers_preview() != LED_LAYERS_NO_PREVIEW ? led_layers_preview() : led_layers_active_layer();
    uint8_t target = data.effect[layer].override ? layer : NO_EFFECT_LAYER;
    if (target == applied_effect) {
        return;
    }
    if (applied_effect == NO_EFFECT_LAYER) {
        base = (base_effect_t){rgb_matrix_is_enabled(), rgb_matrix_get_mode(), rgb_matrix_get_hsv(), rgb_matrix_get_speed()};
    }
    if (target == NO_EFFECT_LAYER) {
        if (base.enable) {
            rgb_matrix_enable_noeeprom();
        } else {
            rgb_matrix_disable_noeeprom();
        }
        rgb_matrix_mode_noeeprom(base.mode);
        rgb_matrix_sethsv_noeeprom(base.hsv.h, base.hsv.s, base.hsv.v);
        rgb_matrix_set_speed_noeeprom(base.speed);
    } else {
        apply_effect(&data.effect[target]);
    }
    applied_effect = target;
}

// --- API -------------------------------------------------------------------------------------------------

void led_layers_init(void) {
    if (!eeconfig_is_kb_datablock_valid()) {
        eeconfig_init_kb_datablock();  // premier démarrage avec ce firmware : couleurs compilées
    } else {
        eeconfig_read_kb_datablock(&data, 0, sizeof(data));
    }
    transaction_register_rpc(RPC_ID_KB_LED_LAYERS, slave_rpc);
}

void led_layers_task(void) {
    if (is_keyboard_master()) {
        if (preview != LED_LAYERS_NO_PREVIEW && timer_elapsed32(preview_time) > PREVIEW_MS) {
            preview = LED_LAYERS_NO_PREVIEW;
        }
        effect_task();
        master_sync_task();
    } else if (preview != LED_LAYERS_NO_PREVIEW && timer_elapsed32(slave_ping_time) > SLAVE_PREVIEW_MS) {
        preview = LED_LAYERS_NO_PREVIEW;  // maître muet (lien coupé) : on revient à la couche active
    }
}

void led_layers_read(uint8_t layer, uint8_t start, uint8_t count, uint8_t *mask, uint8_t *rgb) {
    *mask = 0;
    for (uint8_t i = 0; i < count; i++) {
        memcpy(&rgb[3 * i], data.rgb[layer][start + i], 3);
        if (is_transparent(layer, start + i)) *mask |= 1 << i;
    }
}

void led_layers_write(uint8_t layer, uint8_t start, uint8_t count, uint8_t mask, const uint8_t *rgb) {
    apply_range(layer, start, count, mask, rgb);
    save(data.rgb[layer][start], count * 3);
    save(data.transparent[layer], sizeof(data.transparent[layer]));
    push_slave_part(layer, start, count);
}

void led_layers_reset(uint8_t layer) {
    for (uint8_t l = 0; l < LED_LAYERS; l++) {
        if (layer == LED_LAYERS || layer == l) {
            load_defaults(l);
            save(data.rgb[l], sizeof(data.rgb[l]));
            save(data.transparent[l], sizeof(data.transparent[l]));
            save(&data.effect[l], sizeof(data.effect[l]));
        }
    }
    invalidate_slave();
    applied_effect = applied_effect == NO_EFFECT_LAYER ? NO_EFFECT_LAYER : 0xFE;  // réappliquer
}

void led_layers_get_effect(uint8_t layer, led_layer_effect_t *effect) {
    *effect = data.effect[layer];
}

void led_layers_set_effect(uint8_t layer, const led_layer_effect_t *effect) {
    data.effect[layer] = *effect;
    save(&data.effect[layer], sizeof(data.effect[layer]));
    if (applied_effect == layer) {
        applied_effect = 0xFE;  // réappliqué au prochain passage, sans recapturer l'effet global
    }
}

void led_layers_set_preview(uint8_t layer) {
    preview      = layer < LED_LAYERS ? layer : LED_LAYERS_NO_PREVIEW;
    preview_time = timer_read32();
    ping_time    = 0;  // transmis tout de suite à l'esclave
}

uint8_t led_layers_preview(void) {
    return preview;
}

uint8_t led_layers_active_layer(void) {
    return get_highest_layer(layer_state | default_layer_state);
}

// --- Rendu (les deux moitiés) ------------------------------------------------------------------------------

bool rgb_matrix_indicators_advanced_kb(uint8_t led_min, uint8_t led_max) {
    if (!rgb_matrix_indicators_advanced_user(led_min, led_max)) {
        return false;
    }
    uint8_t layer = preview != LED_LAYERS_NO_PREVIEW ? preview : led_layers_active_layer();
    if (layer >= LED_LAYERS) {
        return false;
    }
    // Même échelle que les effets : courbe CIE 1931 (valeur d'un blanc à la luminosité courante)
    uint8_t peak = hsv_to_rgb((hsv_t){0, 0, rgb_matrix_get_val()}).r;
    for (uint8_t led = led_min; led < led_max; led++) {
        if (is_transparent(layer, led)) {
            continue;
        }
        const uint8_t *c = data.rgb[layer][led];
        rgb_matrix_set_color(led, c[0] * peak / 255, c[1] * peak / 255, c[2] * peak / 255);
    }
    return false;
}
