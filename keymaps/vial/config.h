// Copyright 2025 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// VIAL Configuration
#define VIAL_KEYBOARD_UID {0x0B, 0x9D, 0x2D, 0x84, 0xA4, 0x6C, 0x04, 0x17}

// Unlock combo: touche en haut à gauche (0,0) + touche en haut à droite (0,5)
#define VIAL_UNLOCK_COMBO_ROWS { 0, 0 }
#define VIAL_UNLOCK_COMBO_COLS { 0, 5 }

// Dynamic keymap configuration
#define DYNAMIC_KEYMAP_LAYER_COUNT 3  // BASE, FN, GAMING

// RGB Matrix Configuration
#define RGB_MATRIX_DEFAULT_VAL 128  // Luminosité par défaut à 50% (128/255)

// Activer les contrôles RGB dans VIAL
#define VIAL_INSECURE
