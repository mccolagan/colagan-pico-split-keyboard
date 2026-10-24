// Copyright 2025 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "split_util.h"

// LED embarquée du Pico (GP25, Pico classique uniquement) : rapide = lien split OK, lent = pas de lien
#define STATUS_LED GP25

// Test du lien entre moitiés : lettres sur la moitié maître (USB), chiffres/F-keys sur l'esclave.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(
        KC_A, KC_B, KC_C, KC_D, KC_E, KC_F,
        KC_G, KC_H, KC_I, KC_J, KC_K, KC_L,
        KC_M, KC_N, KC_O, KC_P, KC_Q, KC_R,
        KC_S, KC_T, KC_U, KC_V, KC_W, KC_X,
        KC_Y, KC_Z, KC_SPC, KC_ENT, QK_BOOT,

        KC_1, KC_2, KC_3, KC_4, KC_5, KC_6,
        KC_7, KC_8, KC_9, KC_0, KC_F1, KC_F2,
        KC_F3, KC_F4, KC_F5, KC_F6, KC_F7, KC_F8,
        KC_F9, KC_F10, KC_F11, KC_F12, KC_MINS, KC_EQL,
        KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, QK_BOOT
    )
};

#ifndef NO_STATUS_LED
static uint32_t key_flash = 0;

void keyboard_post_init_user(void) {
    gpio_set_pin_output(STATUS_LED);
}

// Toute touche détectée (maître ou esclave) allume la LED 300 ms
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        key_flash = timer_read32() | 1;
    }
    return true;
}

void housekeeping_task_user(void) {
    static uint32_t last = 0;

    if (key_flash && timer_elapsed32(key_flash) < 300) {
        gpio_write_pin_high(STATUS_LED);
        return;
    }

    uint32_t period = is_transport_connected() ? 100 : 1000;
    if (timer_elapsed32(last) >= period) {
        last = timer_read32();
        gpio_toggle_pin(STATUS_LED);
    }
}
#endif
