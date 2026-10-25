// Copyright 2025 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#define SERIAL_PIO_USE_PIO1

// Le PCB est réversible : le Pico est monté à l'envers d'une moitié à l'autre, donc les GPIO diffèrent.
// Brochage déduit du PCB V2 (net -> pastille -> GPIO). Moitié droite = celle du firmware right_only d'origine.
// Sans HALF_LEFT/HALF_RIGHT : banc de test avec Pico libres reliés par un fil sur GP26.
#if defined(HALF_LEFT) && !defined(HALF_LEFT_RIGHTPINS)
#    define MATRIX_COL_PINS { GP11, GP9, GP19, GP20, GP21, GP22 }
#    define MATRIX_ROW_PINS { GP10, GP12, GP13, GP14, GP15 }
#    ifdef LED_PIN_TEST
#        define WS2812_DI_PIN LED_PIN_TEST  // essai d'un autre GPIO pour la ligne LED
#    else
#        define WS2812_DI_PIN GP18
#    endif
#    define SERIAL_USART_TX_PIN GP26  // net SDA
#elif defined(HALF_RIGHT) || defined(HALF_LEFT_RIGHTPINS)
#    define SERIAL_USART_TX_PIN GP7   // net SDA
#else
#    define SERIAL_USART_TX_PIN GP26
#endif

// RGB : 29 LED par moitié, arc-en-ciel par défaut, luminosité basse (budget courant USB 500 mA)
#define RGB_MATRIX_SPLIT { 29, 29 }
#define RGB_MATRIX_DEFAULT_MODE RGB_MATRIX_CYCLE_LEFT_RIGHT
#define RGB_MATRIX_DEFAULT_VAL 40
#define RGB_MATRIX_DEFAULT_SPD 60
#define WS2812_PIO_USE_PIO0
