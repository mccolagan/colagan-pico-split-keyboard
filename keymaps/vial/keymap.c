// Copyright 2025 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// ============================================================================
// LAYER DEFINITIONS
// ============================================================================

enum layer_names {
    _BASE,      // Côté droit du clavier AZERTY
    _FN,        // Côté gauche du clavier AZERTY (double-tap Ctrl)
    _GAMING     // Layout gaming avec ZQSD et RGB personnalisé (triple-tap Ctrl)
};

// ============================================================================
// CUSTOM KEYCODES
// ============================================================================

enum custom_keycodes {
    CTRL_TOG = SAFE_RANGE,  // Multi-tap: 1x=Ctrl, 2x=toggle FN, 3x=toggle GAMING
    RGB_BR_UP,              // Augmenter la luminosité RGB
    RGB_BR_DN               // Diminuer la luminosité RGB
};

// Variables pour le multi-tap
static uint16_t ctrl_timer = 0;
static uint8_t ctrl_tap_count = 0;

// ============================================================================
// KEYMAPS
// ============================================================================

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Layer BASE - Côté DROIT AZERTY
     * ┌─────┬─────┬─────┬─────┬─────┬─────┐
     * │  -  │  è  │  _  │  ç  │  à  │  )  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │  Y  │  U  │  I  │  O  │  P  │  ^  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │  H  │  J  │  K  │  L  │  M  │  ù  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │  B  │  N  │  ,  │  ;  │  :  │  !  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │ Del │Bksp │ Ent │ Alt │Ctrl │     │
     * │     │     │     │     │ 2xFN│     │
     * └─────┴─────┴─────┴─────┴─────┴─────┘
     */
    [_BASE] = LAYOUT(
        KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_MINS,
        KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC,
        KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT,
        KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
        KC_DEL,  KC_BSPC, KC_ENT,  KC_LALT, CTRL_TOG
    ),

    /* Layer FN - Côté GAUCHE AZERTY
     * ┌─────┬─────┬─────┬─────┬─────┬─────┐
     * │  ²  │  &  │  é  │  "  │  '  │  (  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │ Tab │  A  │  Z  │  E  │  R  │  T  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │Caps │  Q  │  S  │  D  │  F  │  G  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │Shift│  <  │  W  │  X  │  C  │  V  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │ Del │Bksp │ Ent │ Alt │Ctrl │     │
     * └─────┴─────┴─────┴─────┴─────┴─────┘
     */
    [_FN] = LAYOUT(
        KC_GRV,  KC_1,    KC_2,    KC_3,    KC_4,    KC_5,
        KC_TAB,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,
        KC_CAPS, KC_A,    KC_S,    KC_D,    KC_F,    KC_G,
        KC_LSFT, KC_NUBS, KC_Z,    KC_X,    KC_C,    KC_V,
        KC_DEL,  KC_BSPC, KC_ENT,  KC_LALT, CTRL_TOG
    ),

    /* Layer GAMING - Layout optimisé pour le jeu
     * Triple-tap sur Ctrl pour activer/désactiver
     *
     * ┌─────┬─────┬─────┬─────┬─────┬─────┐
     * │  &  │  é  │  "  │  '  │  (  │     │ (bleu/violet)
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │Ctrl │Shift│  Z  │  E  │     │     │ (jaune/blanc)
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │     │  Q  │  S  │  D  │     │     │ (blanc - ZQSD)
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │     │     │     │     │     │     │ (éteint)
     * ├─────┼─────┼─────┼─────┼─────┼─────┘
     * │ Alt │  R  │Space│ Br- │ Br+ │ (rouge/violet/blanc)
     * └─────┴─────┴─────┴─────┴─────┘
     */
    [_GAMING] = LAYOUT(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_TRNS,
        KC_LCTL, KC_LSFT, KC_W,    KC_E,    KC_TRNS, KC_TRNS,
        KC_TRNS, KC_A,    KC_S,    KC_D,    KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_LALT, KC_R,    KC_SPC,  RGB_BR_DN, RGB_BR_UP
    )
};

// ============================================================================
// KEYCODE PROCESSING
// ============================================================================

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
                    // Double-tap: Toggle layer FN
                    if (IS_LAYER_ON(_FN)) {
                        layer_off(_FN);
                    } else {
                        layer_on(_FN);
                        layer_off(_GAMING);
                    }
                } else if (ctrl_tap_count == 3) {
                    // Triple-tap: Toggle layer GAMING
                    if (IS_LAYER_ON(_GAMING)) {
                        layer_off(_GAMING);
                    } else {
                        layer_on(_GAMING);
                        layer_off(_FN);
                    }
                    ctrl_tap_count = 0;
                } else {
                    // Premier tap: Envoyer Ctrl
                    register_code(KC_LCTL);
                }
            } else {
                unregister_code(KC_LCTL);
            }
            return false;

        case RGB_BR_UP:
            if (record->event.pressed) {
                rgb_matrix_increase_val();
            }
            return false;

        case RGB_BR_DN:
            if (record->event.pressed) {
                rgb_matrix_decrease_val();
            }
            return false;
    }
    return true;
}

// ============================================================================
// RGB CONFIGURATION
// ============================================================================

// Configuration RGB par layer
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _GAMING:
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
            break;
        default:
            break;
    }
    return state;
}

// Couleurs RGB personnalisées pour le layer GAMING
bool rgb_matrix_indicators_user(void) {
    if (IS_LAYER_ON(_GAMING)) {
        uint8_t val = rgb_matrix_get_val();

        // Éteindre toutes les LEDs
        rgb_matrix_set_color_all(0, 0, 0);

        // Ligne 0: Chiffres (&, é, ", ', ()
        rgb_matrix_set_color(0, 0, 0, val);        // & - BLEU
        rgb_matrix_set_color(1, 0, 0, val);        // é - BLEU
        rgb_matrix_set_color(2, val, 0, val);      // " - VIOLET
        rgb_matrix_set_color(3, val, 0, val);      // ' - VIOLET
        rgb_matrix_set_color(4, val, 0, val);      // ( - VIOLET

        // Ligne 1: Modificateurs et mouvement vertical (Ctrl, Shift, Z, E)
        rgb_matrix_set_color(6, val, val, 0);      // Ctrl - JAUNE
        rgb_matrix_set_color(7, val, val, 0);      // Shift - JAUNE
        rgb_matrix_set_color(8, val, val, val);    // Z - BLANC
        rgb_matrix_set_color(9, val, val, 0);      // E - JAUNE

        // Ligne 2: ZQSD (Q, S, D)
        rgb_matrix_set_color(13, val, val, val);   // Q - BLANC
        rgb_matrix_set_color(14, val, val, val);   // S - BLANC
        rgb_matrix_set_color(15, val, val, val);   // D - BLANC

        // Ligne 4: Actions (Alt, R, Espace, Brightness)
        rgb_matrix_set_color(24, val, 0, 0);       // Alt - ROUGE
        rgb_matrix_set_color(25, val, 0, val);     // R - VIOLET
        rgb_matrix_set_color(26, val, val, val);   // Espace - BLANC
        rgb_matrix_set_color(27, val/5, val/5, val/5);  // Br- - GRIS
        rgb_matrix_set_color(28, val, val, val);   // Br+ - BLANC
    }
    return false;
}
