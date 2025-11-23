// Copyright 2025 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

// Keymap pour côté DROIT uniquement - Layout AZERTY FR
// Layer BASE : Partie droite AZERTY
// Layer FN : Partie gauche AZERTY (double-tap Ctrl pour toggle)

#include QMK_KEYBOARD_H

enum layer_names {
    _BASE,
    _FN,
    _GAMING
};

enum custom_keycodes {
    CTRL_TOG = SAFE_RANGE
};

// Variables pour le multi-tap (double = FN, triple = GAMING)
static uint16_t ctrl_timer = 0;
static uint8_t ctrl_tap_count = 0;

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
    ),

    /* Gaming Layer - ZQSD au centre (I-J-K-L) avec RGB blanc
     * ,---------------------------------------------------.
     * | --- | --- | --- | --- | --- | --- |  (LEDs OFF)
     * |-----+-----+-----+-----+-----+-----|
     * | --- | --- |  Z  | --- | --- | --- |  (I=Z blanc)
     * |-----+-----+-----+-----+-----+-----|
     * | --- |  Q  |  S  |  D  | --- | --- |  (J=Q K=S L=D blanc)
     * |-----+-----+-----+-----+-----+-----|
     * | --- | --- | --- | --- | --- | --- |  (LEDs OFF)
     * |-----+-----+-----+-----+-----+-----|
     * | --- | --- | --- | --- |Ctrl | --- |  (Triple-tap pour toggle)
     * |     |     |     |     |3xGM |     |
     * `---------------------------------------------------'
     */
    [_GAMING] = LAYOUT(
        // Ligne 0: Tout transparent (LEDs OFF)
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        // Ligne 1: Y=Ctrl(jaune), U=Shift(jaune), I=Z(blanc), O=E(jaune)
        KC_LCTL, KC_LSFT, KC_W,    KC_E,    KC_TRNS, KC_TRNS,
        // Ligne 2: J=Q(blanc), K=S(blanc), L=D(blanc)
        KC_TRNS, KC_A,    KC_S,    KC_D,    KC_TRNS, KC_TRNS,
        // Ligne 3: Tout transparent (LEDs OFF)
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        // Ligne 4: Ctrl toggle pour sortir
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, CTRL_TOG
    )
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case CTRL_TOG:
            if (record->event.pressed) {
                // Vérifier si on est dans la fenêtre de multi-tap
                if (timer_elapsed(ctrl_timer) < TAPPING_TERM) {
                    ctrl_tap_count++;
                } else {
                    ctrl_tap_count = 1;
                }

                ctrl_timer = timer_read();

                if (ctrl_tap_count == 2) {
                    // Double-tap = Toggle layer FN
                    if (IS_LAYER_ON(_FN)) {
                        layer_off(_FN);
                    } else {
                        layer_on(_FN);
                        layer_off(_GAMING);  // Désactive GAMING si actif
                    }
                } else if (ctrl_tap_count == 3) {
                    // Triple-tap = Toggle layer GAMING
                    if (IS_LAYER_ON(_GAMING)) {
                        layer_off(_GAMING);
                    } else {
                        layer_on(_GAMING);
                        layer_off(_FN);  // Désactive FN si actif
                    }
                    ctrl_tap_count = 0;  // Reset le compteur
                } else {
                    // Premier tap - Envoie Ctrl
                    register_code(KC_LCTL);
                }
            } else {
                // Relâchement de la touche
                unregister_code(KC_LCTL);
            }
            return false;
    }
    return true;
}

// Configuration RGB par layer
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _GAMING:
            // Mode gaming : RGB éteint par défaut, sera géré par rgb_matrix_indicators_user
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            rgb_matrix_sethsv_noeeprom(0, 0, 0);  // Tout noir
            break;
        default:
            // Layers normaux : restaure le mode RGB normal
            break;
    }
    return state;
}

// Allumer les touches gaming avec couleurs personnalisées
bool rgb_matrix_indicators_user(void) {
    if (IS_LAYER_ON(_GAMING)) {
        // Éteindre toutes les LEDs
        rgb_matrix_set_color_all(0, 0, 0);

        // Ligne 1: Y=Ctrl(jaune), U=Shift(jaune), I=Z(blanc), O=E(jaune)
        rgb_matrix_set_color(6, 255, 255, 0);     // LED 6 = Y = Ctrl (JAUNE)
        rgb_matrix_set_color(7, 255, 255, 0);     // LED 7 = U = Shift (JAUNE)
        rgb_matrix_set_color(8, 255, 255, 255);   // LED 8 = I = Z (BLANC)
        rgb_matrix_set_color(9, 255, 255, 0);     // LED 9 = O = E (JAUNE)

        // Ligne 2: J=Q, K=S, L=D (blanc)
        rgb_matrix_set_color(13, 255, 255, 255);  // LED 13 = J = Q (BLANC)
        rgb_matrix_set_color(14, 255, 255, 255);  // LED 14 = K = S (BLANC)
        rgb_matrix_set_color(15, 255, 255, 255);  // LED 15 = L = D (BLANC)
    }
    return false;
}
