// Copyright 2025 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

// Keymap pour côté DROIT uniquement - Layout AZERTY FR
// Layer BASE : Partie droite AZERTY
// Layer FN : Partie gauche AZERTY (double-tap Ctrl pour toggle)

#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
    _FN
};

enum custom_keycodes {
    CTRL_TOG = SAFE_RANGE
};

// Variables pour le double-tap
static uint16_t ctrl_timer = 0;
static bool ctrl_tapped = false;

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base Layer - Côté DROIT
     * ,---------------------------------------------------.
     * |  -  |  è  |  _  |  ç  |  à  |  )  |
     * |-----+-----+-----+-----+-----+-----|
     * |  Y  |  U  |  I  |  O  |  P  |  ^  |
     * |-----+-----+-----+-----+-----+-----|
     * |  H  |  J  |  K  |  L  |  M  |  ù  |
     * |-----+-----+-----+-----+-----+-----|
     * |  B  |  N  |  ,  |  ;  |  :  |  !  |
     * |-----+-----+-----+-----+-----+-----|
     * | Del |Bksp | Ent | Alt |Ctrl |  X  |
     * |     |     |     |     |2xFN |     |
     * `---------------------------------------------------'
     */

    [_BASE] = LAYOUT(
        // Ligne 1: -, è, _, ç, à, ) (pour OS AZERTY)
        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
        // Ligne 2: y, u, i, o, p, ^
        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
        // Ligne 3: h, j, k, l, m, ù (AZERTY FR)
        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        // Ligne 4: b, n, ,, ;, :, ! (AZERTY FR)
        KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
        // Ligne 5: Del, Backspace, Enter, Alt, Ctrl (double-tap toggle)
        KC_DEL,  KC_BSPC, KC_ENT,  KC_LALT, CTRL_TOG
    ),

    /* Function Layer - Côté GAUCHE AZERTY (pour OS AZERTY)
     * ,---------------------------------------------------.
     * |  ²  |  &  |  é  |  "  |  '  |  (  |
     * |-----+-----+-----+-----+-----+-----|
     * | Tab |  A  |  Z  |  E  |  R  |  T  |
     * |-----+-----+-----+-----+-----+-----|
     * |Caps |  Q  |  S  |  D  |  F  |  G  |
     * |-----+-----+-----+-----+-----+-----|
     * |Shift|  <  |  W  |  X  |  C  |  V  |
     * |-----+-----+-----+-----+-----+-----|
     * | Del |Bksp | Ent | Alt | /// |  X  |
     * `---------------------------------------------------'
     */
    [_FN] = LAYOUT(
        // Ligne 1: ², &, é, ", ', ( (pour OS AZERTY)
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,
        // Ligne 2: Tab, A, Z, E, R, T (pour OS AZERTY)
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
        // Ligne 3: Caps Lock, Q, S, D, F, G (pour OS AZERTY)
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
        // Ligne 4: Shift, <, W, X, C, V (pour OS AZERTY)
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,
        // Ligne 5: Del, Backspace, Enter, Alt, Ctrl toggle
        KC_DEL,  KC_BSPC, KC_ENT,  KC_LALT, CTRL_TOG
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTRL_TOG:
            if (record->event.pressed) {
                // Premier tap ou double-tap ?
                if (ctrl_tapped && timer_elapsed(ctrl_timer) < TAPPING_TERM) {
                    // Double-tap détecté ! Toggle layer FN
                    if (IS_LAYER_ON(_FN)) {
                        layer_off(_FN);
                    } else {
                        layer_on(_FN);
                    }
                    ctrl_tapped = false;
                } else {
                    // Premier tap - Envoie Ctrl
                    register_code(KC_LCTL);
                    ctrl_timer = timer_read();
                    ctrl_tapped = true;
                }
            } else {
                // Relâchement de la touche
                unregister_code(KC_LCTL);
            }
            return false;
    }
    return true;
}
