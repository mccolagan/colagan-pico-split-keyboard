// Copyright 2026 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "led_layers.h"

// Keymap par défaut : chargée au premier démarrage ou après un « Reset » dans Vial. Ensuite, les touches
// se modifient dans Vial (enregistrées dans la mémoire de la droite) ; ce fichier ne sert plus qu'aux couleurs.
// Keycodes QWERTY, OS en AZERTY : KC_Q écrit « a », KC_W « z », KC_SCLN « m », KC_M « , ».

enum layer_names {
    _BASE,    // Frappe normale (AZERTY)
    _FN,      // Pouce gauche tenu : F1-F12, flèches, médias
    _FPS,     // Warzone : touches par défaut du jeu sous la main droite
    _MMO,     // WoW : emplacements de sorts + modificateurs aux pouces
    _MMO_UI,  // Tenue depuis MMO : F1-F5 et fenêtres du jeu
    _SEL,     // 3 appuis rapides sur le pouce droit extérieur : choix de la couche, réglages RGB, bootloader
    // Couches 6 et 7 libres, à remplir dans Vial
};

#define ___ KC_TRNS
#define XXX KC_NO
#define KC_FSLS S(KC_DOT)  // « / » en AZERTY

// Touche personnalisée (« SEL x3 » dans l'onglet User de Vial) : 3 appuis en moins de 500 ms ouvrent le
// sélecteur de couche, pour éviter les changements de couche accidentels en jeu. N'envoie rien d'autre.
enum custom_keycodes {
    SEL_X3 = QK_KB_0,
};
#define SEL_TAP_TERM 500

// Disposition : chaque rangée = 6 touches gauche (du bord vers le centre) + 6 touches droite
// (du centre vers le bord, colonne extérieure en 1,5U) ; dernière ligne = 5 pouces gauche + 5 pouces droite.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        KC_ESC,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,        KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                 KC_LGUI, KC_LALT, KC_LCTL, KC_SPC,  MO(_FN),     KC_DEL,  KC_BSPC, KC_ENT,  KC_RALT, SEL_X3
    ),

    [_FN] = LAYOUT(
        KC_GRV,  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,       KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_INS,  KC_F12,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_NUHS, KC_RBRC,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_MPLY, KC_VOLD, KC_VOLU, KC_MUTE, KC_EQL,  KC_PSCR,
                 ___,     ___,     ___,     ___,     ___,         ___,     ___,     ___,     ___,     ___
    ),

    // Cluster ZQSD identique sur FPS et MMO ; menus (Bksp, Entrée, Échap, /) au même endroit.
    [_FPS] = LAYOUT(
        ___,     ___,     ___,     ___,     ___,     ___,         KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_TAB,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_C,    KC_LSFT, KC_W,    KC_E,    KC_F,    KC_G,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_X,    KC_Q,    KC_S,    KC_D,    KC_R,    KC_V,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_SCLN, KC_T,    KC_FSLS, KC_BSPC, KC_ENT,  KC_ESC,
                 ___,     ___,     ___,     ___,     ___,         KC_SPC,  KC_LCTL, KC_LALT, XXX,     SEL_X3
    ),

    [_MMO] = LAYOUT(
        ___,     ___,     ___,     ___,     ___,     ___,         KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_7,    KC_8,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_NUM,  KC_A,    KC_S,    KC_D,    KC_9,    KC_0,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_MINS, MO(_MMO_UI), KC_FSLS, KC_BSPC, KC_ENT, KC_ESC,
                 ___,     ___,     ___,     ___,     ___,         KC_SPC,  KC_LSFT, KC_LCTL, KC_LALT, SEL_X3
    ),

    [_MMO_UI] = LAYOUT(
        ___,     ___,     ___,     ___,     ___,     ___,         KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_P,
        ___,     ___,     ___,     ___,     ___,     ___,         ___,     ___,     ___,     ___,     KC_C,    KC_B,
        ___,     ___,     ___,     ___,     ___,     ___,         ___,     ___,     ___,     ___,     KC_SCLN, KC_N,
        ___,     ___,     ___,     ___,     ___,     ___,         KC_L,    ___,     ___,     ___,     ___,     ___,
                 ___,     ___,     ___,     ___,     ___,         ___,     ___,     ___,     ___,     ___
    ),

    // Ouvert par SEL_X3 ; une touche TO() le referme, ou un appui sur le pouce. QK_BOOT = bootloader (droite)
    [_SEL] = LAYOUT(
        XXX,     XXX,     XXX,     XXX,     XXX,     XXX,         TO(_BASE), TO(_FPS), TO(_MMO), XXX,   XXX,     XXX,
        XXX,     XXX,     XXX,     XXX,     XXX,     XXX,         RM_VALD, RM_VALU, RM_NEXT, RM_TOGG, XXX,     XXX,
        XXX,     XXX,     XXX,     XXX,     XXX,     XXX,         XXX,     XXX,     XXX,     XXX,     XXX,     XXX,
        XXX,     XXX,     XXX,     XXX,     XXX,     XXX,         XXX,     XXX,     XXX,     XXX,     XXX,     QK_BOOT,
                 XXX,     XXX,     XXX,     XXX,     XXX,         XXX,     XXX,     XXX,     XXX,     ___
    ),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    static uint16_t first_tap = 0;
    static uint8_t  taps      = 0;

    if (keycode != SEL_X3) {
        return true;
    }
    if (record->event.pressed) {
        if (IS_LAYER_ON(_SEL)) {
            layer_off(_SEL);  // un appui referme le sélecteur sans changer de couche
            taps = 0;
        } else {
            if (taps == 0 || timer_elapsed(first_tap) > SEL_TAP_TERM) {
                taps      = 0;
                first_tap = timer_read();
            }
            if (++taps == 3) {
                layer_on(_SEL);
                taps = 0;
            }
        }
    }
    return false;
}

// ============================================================================
// COULEURS PAR COUCHE : VALEURS PAR DÉFAUT
// Même disposition que les keymaps. __ = laisser l'effet RGB ; OFF = éteinte.
// Chargées seulement si la mémoire du clavier n'a pas encore de couleurs (premier démarrage, bootmagic) :
// au quotidien, couleurs et effet par couche se règlent dans le configurateur web.
// ============================================================================

enum led_colors { __, OFF, RED, ORG, YEL, GRN, BLU, PUR, WHT, GRY };

static const uint8_t PROGMEM palette[][3] = {
    [OFF] = {0, 0, 0},
    [RED] = {255, 0, 0},
    [ORG] = {255, 100, 0},
    [YEL] = {255, 255, 0},
    [GRN] = {0, 255, 0},
    [BLU] = {0, 0, 255},
    [PUR] = {255, 0, 255},
    [WHT] = {255, 255, 255},
    [GRY] = {50, 50, 50},
};

static const uint8_t PROGMEM ledmaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_FPS] = LAYOUT(
        OFF, OFF, OFF, OFF, OFF, OFF,     BLU, BLU, BLU, BLU, BLU, YEL,  // 1-5 bleu, Tab
        OFF, OFF, OFF, OFF, OFF, OFF,     RED, YEL, WHT, RED, RED, RED,  // Shift jaune, Z blanc, actions rouge
        OFF, OFF, OFF, OFF, OFF, OFF,     RED, WHT, WHT, WHT, RED, RED,  // QSD blanc
        OFF, OFF, OFF, OFF, OFF, OFF,     PUR, PUR, PUR, PUR, PUR, PUR,  // menus violet
             OFF, OFF, OFF, OFF, OFF,     WHT, YEL, RED, OFF, GRN        // Espace, Ctrl, Alt, sélecteur
    ),
    [_MMO] = LAYOUT(
        OFF, OFF, OFF, OFF, OFF, OFF,     BLU, BLU, BLU, BLU, BLU, BLU,  // sorts bleu
        OFF, OFF, OFF, OFF, OFF, OFF,     YEL, GRY, WHT, GRY, BLU, BLU,  // Tab, strafe A/E gris
        OFF, OFF, OFF, OFF, OFF, OFF,     YEL, WHT, WHT, WHT, BLU, BLU,  // autorun, QSD blanc
        OFF, OFF, OFF, OFF, OFF, OFF,     BLU, GRN, PUR, PUR, PUR, PUR,  // UI vert, menus violet
             OFF, OFF, OFF, OFF, OFF,     WHT, YEL, YEL, YEL, GRN        // Espace, modificateurs, sélecteur
    ),
    [_MMO_UI] = LAYOUT(
        OFF, OFF, OFF, OFF, OFF, OFF,     GRN, GRN, GRN, GRN, GRN, GRN,  // F1-F5, fenêtres vert
        OFF, OFF, OFF, OFF, OFF, OFF,     OFF, OFF, WHT, OFF, GRN, GRN,
        OFF, OFF, OFF, OFF, OFF, OFF,     OFF, WHT, WHT, WHT, GRN, GRN,
        OFF, OFF, OFF, OFF, OFF, OFF,     GRN, OFF, OFF, OFF, OFF, OFF,
             OFF, OFF, OFF, OFF, OFF,     WHT, OFF, OFF, OFF, OFF
    ),
    // Sélecteur : seules les touches utiles sont colorées, les autres montrent l'effet en cours (aperçu)
    [_SEL] = LAYOUT(
        __ , __ , __ , __ , __ , __ ,     WHT, RED, BLU, __ , __ , __ ,  // BASE, FPS, MMO
        __ , __ , __ , __ , __ , __ ,     WHT, WHT, WHT, WHT, __ , __ ,  // luminosité -/+, effet suivant, on/off
        __ , __ , __ , __ , __ , __ ,     __ , __ , __ , __ , __ , __ ,
        __ , __ , __ , __ , __ , __ ,     __ , __ , __ , __ , __ , ORG,  // bootloader
             __ , __ , __ , __ , __ ,     __ , __ , __ , __ , GRN
    ),
};

// Couleurs par défaut des couches (premier démarrage ou EEPROM effacée) : ensuite elles se modifient
// dans le configurateur web et sont gardées dans la mémoire du clavier (led_layers.c).
bool led_layers_default_color(uint8_t layer, uint8_t row, uint8_t col, uint8_t rgb[3]) {
    if (layer >= ARRAY_SIZE(ledmaps)) {
        return false;
    }
    uint8_t color = pgm_read_byte(&ledmaps[layer][row][col]);
    if (color == __) {
        return false;
    }
    for (uint8_t i = 0; i < 3; i++) {
        rgb[i] = pgm_read_byte(&palette[color][i]);
    }
    return true;
}
