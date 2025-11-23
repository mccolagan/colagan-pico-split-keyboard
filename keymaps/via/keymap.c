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
    CTRL_TOG = SAFE_RANGE,
    RGB_BR_UP,    // Custom pour augmenter luminosité
    RGB_BR_DN     // Custom pour diminuer luminosité
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
        // Ligne 0: &(bleu), é(bleu), "(violet), '(violet), ((violet), reste OFF
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_TRNS,
        // Ligne 1: Y=Ctrl(jaune), U=Shift(jaune), I=Z(blanc), O=E(jaune)
        KC_LCTL, KC_LSFT, KC_W,    KC_E,    KC_TRNS, KC_TRNS,
        // Ligne 2: J=Q(blanc), K=S(blanc), L=D(blanc)
        KC_TRNS, KC_A,    KC_S,    KC_D,    KC_TRNS, KC_TRNS,
        // Ligne 3: Tout transparent (LEDs OFF)
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        // Ligne 4: Alt gauche(rouge), R(violet), Espace(blanc), Brightness-, Brightness+
        KC_LALT, KC_R,    KC_SPC,  RGB_BR_DN, RGB_BR_UP
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

// Configuration RGB par layer
layer_state_t layer_state_set_user(layer_state_t state) {
    switch (get_highest_layer(state)) {
        case _GAMING:
            // Mode gaming : couleurs custom gérées par rgb_matrix_indicators_user
            rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
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
        // Récupérer la luminosité actuelle (0-255)
        uint8_t val = rgb_matrix_get_val();

        // Éteindre toutes les LEDs
        rgb_matrix_set_color_all(0, 0, 0);

        // Appliquer la luminosité à chaque couleur
        // Ligne 0: &(bleu), é(bleu), "(violet), '(violet), ((violet)
        rgb_matrix_set_color(0, 0, 0, val);                    // LED 0 = - = & (BLEU)
        rgb_matrix_set_color(1, 0, 0, val);                    // LED 1 = è = é (BLEU)
        rgb_matrix_set_color(2, val, 0, val);                  // LED 2 = _ = " (VIOLET/MAGENTA)
        rgb_matrix_set_color(3, val, 0, val);                  // LED 3 = ç = ' (VIOLET/MAGENTA)
        rgb_matrix_set_color(4, val, 0, val);                  // LED 4 = à = ( (VIOLET/MAGENTA)

        // Ligne 1: Y=Ctrl(jaune), U=Shift(jaune), I=Z(blanc), O=E(jaune)
        rgb_matrix_set_color(6, val, val, 0);                  // LED 6 = Y = Ctrl (JAUNE)
        rgb_matrix_set_color(7, val, val, 0);                  // LED 7 = U = Shift (JAUNE)
        rgb_matrix_set_color(8, val, val, val);                // LED 8 = I = Z (BLANC)
        rgb_matrix_set_color(9, val, val, 0);                  // LED 9 = O = E (JAUNE)

        // Ligne 2: J=Q(blanc), K=S(blanc), L=D(blanc)
        rgb_matrix_set_color(13, val, val, val);               // LED 13 = J = Q (BLANC)
        rgb_matrix_set_color(14, val, val, val);               // LED 14 = K = S (BLANC)
        rgb_matrix_set_color(15, val, val, val);               // LED 15 = L = D (BLANC)

        // Ligne 4: Del=Alt(rouge), Bksp=R(violet), Enter=Espace(blanc)
        rgb_matrix_set_color(24, val, 0, 0);                   // LED 24 = Del = Alt gauche (ROUGE)
        rgb_matrix_set_color(25, val, 0, val);                 // LED 25 = Bksp = R (VIOLET/MAGENTA)
        rgb_matrix_set_color(26, val, 0, 0);                   // LED 26 = Enter = Espace (ROUGE)
        rgb_matrix_set_color(27, val/5, val/5, val/5);         // LED 27 = Brightness- (gris)
        rgb_matrix_set_color(28, val, val, val);               // LED 28 = Brightness+ (blanc)
    }
    return false;
}
