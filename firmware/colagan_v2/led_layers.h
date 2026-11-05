// Copyright 2026 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdbool.h>
#include <stdint.h>

#define LED_LAYERS 8          // = DYNAMIC_KEYMAP_LAYER_COUNT (vérifié dans led_layers.c)
#define LED_LAYERS_LEDS 58    // = RGB_MATRIX_LED_COUNT
#define LED_LAYERS_RANGE_MAX 8 // LED par lecture / écriture (raw HID et RPC split)
#define LED_LAYERS_NO_PREVIEW 0xFF

// Effet propre à une couche ; override = 0 : la couche garde l'effet global (réglé par VialRGB).
typedef struct __attribute__((packed)) {
    uint8_t  override;
    uint16_t vialrgb_id;  // identifiant VialRGB (0 = éteint), comme la commande VialRGB 0x41
    uint8_t  speed;
    uint8_t  hue;
    uint8_t  sat;
    uint8_t  val;
} led_layer_effect_t;

void led_layers_init(void);
void led_layers_task(void);

// Couleurs : count ≤ LED_LAYERS_RANGE_MAX ; bit i de mask = LED start+i laisse voir l'effet (transparente).
void led_layers_read(uint8_t layer, uint8_t start, uint8_t count, uint8_t *mask, uint8_t *rgb);
void led_layers_write(uint8_t layer, uint8_t start, uint8_t count, uint8_t mask, const uint8_t *rgb);
void led_layers_reset(uint8_t layer);  // couleurs et effet par défaut (compilés) ; LED_LAYERS = toutes

void led_layers_get_effect(uint8_t layer, led_layer_effect_t *effect);
void led_layers_set_effect(uint8_t layer, const led_layer_effect_t *effect);

// Aperçu : le clavier affiche l'éclairage de cette couche (sans changer la couche de frappe) pendant
// LED_LAYERS_PREVIEW_MS ; à renouveler par le configurateur. LED_LAYERS_NO_PREVIEW pour arrêter.
void    led_layers_set_preview(uint8_t layer);
uint8_t led_layers_preview(void);
uint8_t led_layers_active_layer(void);

// Couleurs par défaut, fournies par la keymap (tableau ledmaps) : false = laisser l'effet.
bool led_layers_default_color(uint8_t layer, uint8_t row, uint8_t col, uint8_t rgb[3]);
