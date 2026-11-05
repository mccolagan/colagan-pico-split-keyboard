// Copyright 2026 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#include "quantum.h"
#include "split_util.h"

void led_stream_init(void);

// QMK ne plafonne la luminosité qu'au réglage, pas au chargement depuis l'EEPROM : une valeur laissée par
// un ancien firmware (ex. 200) dépasserait le budget USB avec 58 LED.
static void clamp_rgb_brightness(void) {
    if (rgb_matrix_get_val() > RGB_MATRIX_MAXIMUM_BRIGHTNESS) {
        rgb_matrix_sethsv(rgb_matrix_get_hue(), rgb_matrix_get_sat(), RGB_MATRIX_MAXIMUM_BRIGHTNESS);
    }
}

#if defined(HALF_LEFT)
void keyboard_post_init_kb(void) {
    clamp_rgb_brightness();
    led_stream_init();
    keyboard_post_init_user();
}

bool is_keyboard_left_impl(void) { return true; }
#else
bool is_keyboard_left_impl(void) { return false; }

// LED du Pico (GP25, Pico classique de la droite) pendant les 10 premières secondes :
// rapide = lien avec la gauche OK, lente = pas de lien. Éteinte ensuite.
#    define STATUS_LED GP25
#    define STATUS_LED_BOOT_MS 10000

void keyboard_post_init_kb(void) {
    gpio_set_pin_output(STATUS_LED);
    clamp_rgb_brightness();
    led_stream_init();
    keyboard_post_init_user();
}

void housekeeping_task_kb(void) {
    static uint32_t last = 0;

    if (timer_read32() > STATUS_LED_BOOT_MS) {
        gpio_write_pin_low(STATUS_LED);
    } else if (timer_elapsed32(last) >= (is_transport_connected() ? 100 : 1000)) {
        last = timer_read32();
        gpio_toggle_pin(STATUS_LED);
    }
    housekeeping_task_user();
}
#endif
