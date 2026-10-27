// Copyright 2025 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

// ============================================================================
// LAYER DEFINITIONS
// ============================================================================

enum layer_names {
    _BASE,      // Côté droit du clavier AZERTY
    _FN,        // Côté gauche du clavier AZERTY (double-tap Ctrl)
    _FPS,       // Warzone : touches par défaut du jeu sous la main droite
    _MMO,       // WoW : emplacements de sorts + modificateurs aux pouces
    _MMO_UI,    // Sous-couche tenue : F1-F5 et fenêtres du jeu (C B M N P L)
    _SEL        // Sélecteur de couche (triple-tap Ctrl ou pouce tenu en jeu)
};

// ============================================================================
// CUSTOM KEYCODES
// ============================================================================

enum custom_keycodes {
    CTRL_TOG = SAFE_RANGE,  // Multi-tap: 1x=Ctrl, 2x=toggle FN, 3x=sélecteur
    RGB_BR_UP,              // Augmenter la luminosité RGB
    RGB_BR_DN               // Diminuer la luminosité RGB
};

// Variables pour le multi-tap
static uint16_t ctrl_timer = 0;
static uint8_t ctrl_tap_count = 0;

// Fenêtre max entre deux taps du multi-tap Ctrl (ms)
#define CTRL_MULTITAP_TERM 500

// Touche de menu commune aux couches de jeu : "/" en AZERTY = Shift + ":"
#define KC_FSLS S(KC_DOT)

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

    /* Layer FPS - Warzone (touches par défaut, AZERTY côté OS)
     * Cluster ZQSD identique à la couche MMO.
     *
     * ┌─────┬─────┬─────┬─────┬─────┬─────┐
     * │  1  │  2  │  3  │  4  │  5  │ Tab │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │  C  │Shift│  Z  │  E  │  F  │  G  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │  X  │  Q  │  S  │  D  │  R  │  V  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │  M  │  T  │  /  │Bksp │ Ent │ Esc │
     * ├─────┼─────┼─────┼─────┼─────┼─────┘
     * │ Spc │Ctrl │ Alt │     │ SEL │
     * └─────┴─────┴─────┴─────┴─────┘
     */
    [_FPS] = LAYOUT(
        KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_TAB,
        KC_C,    KC_LSFT, KC_W,    KC_E,    KC_F,    KC_G,
        KC_X,    KC_A,    KC_S,    KC_D,    KC_R,    KC_V,
        KC_M,    KC_T,    KC_FSLS, KC_BSPC, KC_ENT,  KC_ESC,
        KC_SPC,  KC_LCTL, KC_LALT, KC_NO,   MO(_SEL)
    ),

    /* Layer MMO - WoW (S1..S11 = emplacements de sorts, à lier dans le jeu)
     * Shift/Ctrl/Alt aux pouces multiplient chaque emplacement.
     *
     * ┌─────┬─────┬─────┬─────┬─────┬─────┐
     * │ S1  │ S2  │ S3  │ S4  │ S5  │ S6  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │ Tab │  A  │  Z  │  E  │ S7  │ S8  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │NumLk│  Q  │  S  │  D  │ S9  │ S10 │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │ S11 │ UI  │  /  │Bksp │ Ent │ Esc │
     * ├─────┼─────┼─────┼─────┼─────┼─────┘
     * │ Spc │Shift│Ctrl │ Alt │ SEL │
     * └─────┴─────┴─────┴─────┴─────┘
     */
    [_MMO] = LAYOUT(
        KC_1,    KC_2,       KC_3,    KC_4,    KC_5,    KC_6,
        KC_TAB,  KC_Q,       KC_W,    KC_E,    KC_7,    KC_8,
        KC_NUM,  KC_A,       KC_S,    KC_D,    KC_9,    KC_0,
        KC_MINS, MO(_MMO_UI), KC_FSLS, KC_BSPC, KC_ENT,  KC_ESC,
        KC_SPC,  KC_LSFT,    KC_LCTL, KC_LALT, MO(_SEL)
    ),

    /* Layer MMO_UI - tenue depuis MMO, le reste de la couche reste actif
     * ┌─────┬─────┬─────┬─────┬─────┬─────┐
     * │ F1  │ F2  │ F3  │ F4  │ F5  │  P  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │     │     │     │     │  C  │  B  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │     │     │     │     │  M  │  N  │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │  L  │     │     │     │     │     │
     * └─────┴─────┴─────┴─────┴─────┴─────┘
     */
    [_MMO_UI] = LAYOUT(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_P,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_C,    KC_B,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_M,    KC_N,
        KC_L,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    /* Layer SEL - choix de la couche (relâcher la touche SEL pour annuler)
     * ┌─────┬─────┬─────┬─────┬─────┬─────┐
     * │BASE │ FPS │ MMO │ FN  │     │     │
     * ├─────┼─────┼─────┼─────┼─────┼─────┤
     * │ Br- │ Br+ │     │     │     │     │
     * └─────┴─────┴─────┴─────┴─────┴─────┘
     */
    [_SEL] = LAYOUT(
        TO(_BASE),  TO(_FPS),   TO(_MMO),   TO(_FN),    KC_NO,   KC_NO,
        RGB_BR_DN,  RGB_BR_UP,  KC_NO,      KC_NO,      KC_NO,   KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,   KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_NO,   KC_NO,
        KC_NO,      KC_NO,      KC_NO,      KC_NO,      KC_TRNS
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
                if (timer_elapsed(ctrl_timer) < CTRL_MULTITAP_TERM) {
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
                    }
                } else if (ctrl_tap_count == 3) {
                    // Triple-tap: Toggle le sélecteur de couche
                    layer_off(_FN);
                    if (IS_LAYER_ON(_SEL)) {
                        layer_off(_SEL);
                    } else {
                        layer_on(_SEL);
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

// Index LED = ligne * 6 + colonne (pouces : 24 à 28)
static void led_group(const uint8_t *leds, uint8_t count, uint8_t r, uint8_t g, uint8_t b) {
    uint8_t val = rgb_matrix_get_val();
    for (uint8_t i = 0; i < count; i++) {
        rgb_matrix_set_color(leds[i], (uint16_t)r * val / 255, (uint16_t)g * val / 255, (uint16_t)b * val / 255);
    }
}

#define LED_GROUP(r, g, b, ...) do { \
    static const uint8_t leds[] = { __VA_ARGS__ }; \
    led_group(leds, sizeof(leds), r, g, b); \
} while (0)

// Couleurs personnalisées par couche de jeu
bool rgb_matrix_indicators_user(void) {
    switch (get_highest_layer(layer_state)) {
        case _FPS:
            rgb_matrix_set_color_all(0, 0, 0);
            LED_GROUP(0, 0, 255,   0, 1, 2, 3, 4);                    // 1-5 - BLEU
            LED_GROUP(255, 255, 0, 5, 7, 25);                         // Tab, Shift, Ctrl - JAUNE
            LED_GROUP(255, 255, 255, 8, 13, 14, 15, 24);              // ZQSD, Espace - BLANC
            LED_GROUP(255, 0, 0,   6, 9, 10, 11, 12, 16, 17, 26);     // Actions, Alt - ROUGE
            LED_GROUP(255, 0, 255, 18, 19, 20, 21, 22, 23);           // Menus - VIOLET
            LED_GROUP(0, 255, 0,   28);                               // Sélecteur - VERT
            break;

        case _MMO:
        case _MMO_UI:
            rgb_matrix_set_color_all(0, 0, 0);
            LED_GROUP(0, 0, 255,   0, 1, 2, 3, 4, 5, 10, 11, 16, 17, 18);  // Sorts S1-S11 - BLEU
            LED_GROUP(255, 255, 255, 8, 13, 14, 15, 24);              // ZQSD, Espace - BLANC
            LED_GROUP(50, 50, 50,  7, 9);                             // Strafe A/E - GRIS
            LED_GROUP(255, 255, 0, 6, 12, 25, 26, 27);                // Tab, autorun, modificateurs - JAUNE
            LED_GROUP(255, 0, 255, 20, 21, 22, 23);                   // Menus - VIOLET
            LED_GROUP(0, 255, 0,   19, 28);                           // UI, sélecteur - VERT
            if (IS_LAYER_ON(_MMO_UI)) {
                rgb_matrix_set_color_all(0, 0, 0);
                LED_GROUP(0, 255, 0, 0, 1, 2, 3, 4, 5, 10, 11, 16, 17, 18);  // F1-F5, fenêtres - VERT
                LED_GROUP(255, 255, 255, 8, 13, 14, 15, 24);          // ZQSD, Espace - BLANC
            }
            break;

        case _SEL:
            rgb_matrix_set_color_all(0, 0, 0);
            LED_GROUP(255, 255, 255, 0, 6, 7);                        // BASE, luminosité - BLANC
            LED_GROUP(255, 0, 0,   1);                                // FPS - ROUGE
            LED_GROUP(0, 0, 255,   2);                                // MMO - BLEU
            LED_GROUP(0, 255, 0,   3);                                // FN - VERT
            break;

        default:
            break;
    }
    return false;
}
