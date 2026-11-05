// Copyright 2026 kebab (@kebab)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// Un build par moitié (HALF_LEFT / HALF_RIGHT) : le PCB est réversible, le Pico est retourné d'une moitié
// à l'autre, donc la matrice, la LED et le fil du lien série ne sont pas sur les mêmes GPIO.
// keyboard.json décrit la droite ; la gauche (Pico W, monté face arrière) surcharge ici.
#if defined(HALF_LEFT)
#    define MATRIX_COL_PINS { GP11, GP9, GP19, GP20, GP21, GP22 }
#    define MATRIX_ROW_PINS { GP10, GP12, GP13, GP14, GP15 }
#    define WS2812_DI_PIN GP18
#    define SERIAL_USART_TX_PIN GP27  // USB-C monté retourné à gauche : D+/D- croisés, SCL gauche = SDA droite
#elif defined(HALF_RIGHT)
#    define SERIAL_USART_TX_PIN GP7
#else
#    error "Compiler avec -DHALF_LEFT ou -DHALF_RIGHT (voir build-vial.sh)"
#endif

// Lien série half-duplex sur un fil (PIO1 ; PIO0 pilote les LED)
#define SERIAL_PIO_USE_PIO1
#define WS2812_PIO_USE_PIO0

// Rôles : la moitié branchée au PC (USB configuré par l'hôte) devient maître, l'autre esclave.
// Délai porté à 5 s pour les PC lents ; un esclave sans maître redémarre et retente (watchdog).
// (VBUS sur GP24 inutilisable pour ça : le +5V passe par le câble, les deux moitiés le voient.)
// La keymap Vial est dans la mémoire du maître : brancher la droite au quotidien.
// Éviter les ports « clavier » des KVM : ils gèrent mal un périphérique composite (clavier + souris + médias + Vial).
#define SPLIT_USB_TIMEOUT 5000
#define SPLIT_WATCHDOG_ENABLE
#define SPLIT_WATCHDOG_TIMEOUT 6000

// L'esclave a besoin de la couche active (couleurs par couche) et des touches du maître (effets réactifs)
#define SPLIT_LAYER_STATE_ENABLE
#define SPLIT_TRANSPORT_MIRROR

// Effets RGB qui réagissent aux touches ou utilisent un framebuffer
#define RGB_MATRIX_KEYPRESSES
#define RGB_MATRIX_FRAMEBUFFER_EFFECTS

// Configurateur (led_stream.c, led_layers.c) : RPC pour lire les LED de la moitié esclave et lui envoyer
// les couleurs par couche
#define SPLIT_TRANSACTION_IDS_KB RPC_ID_KB_LED_STREAM, RPC_ID_KB_LED_LAYERS

// Couleurs et effet par couche (led_layers.c) : 8 × 58 × 3 + 8 × 8 + 8 × 7 octets dans le bloc clavier.
// Version propre : un ancien contenu (autre firmware) est remplacé par les couleurs compilées.
#define EECONFIG_KB_DATA_SIZE 1512
#define EECONFIG_KB_DATA_VERSION 0xC0A1D002

// EEPROM émulée doublée (16 Ko de flash, 8 Ko utiles) pour garder la place des macros Vial
#define WEAR_LEVELING_BACKING_SIZE 16384
