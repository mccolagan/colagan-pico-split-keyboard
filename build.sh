#!/bin/bash
# Script de compilation du firmware Colagan V2 Right Only
# Chemins surchargeables : QMK_DIR (clone de qmk_firmware) et QMK_BIN (CLI qmk)

set -e  # Arrêter en cas d'erreur

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_DIR="${QMK_DIR:-$PROJECT_DIR/../qmk_firmware}"
QMK_BIN="${QMK_BIN:-$PROJECT_DIR/../qmk_env/bin/qmk}"
KB_DIR="$QMK_DIR/keyboards/colagan_v2_right_only"

echo "📋 Synchronisation des fichiers keymap..."
cp "$PROJECT_DIR/keymaps/via/keymap.c" "$KB_DIR/keymaps/via/keymap.c"
cp "$PROJECT_DIR/keymaps/via/config.h" "$KB_DIR/keymaps/via/config.h"
cp "$PROJECT_DIR/keyboard.json" "$KB_DIR/keyboard.json"

echo "🔨 Compilation du firmware..."
cd "$QMK_DIR"
"$QMK_BIN" compile -kb colagan_v2_right_only -km via

echo "📦 Copie du firmware dans le projet..."
cp "$QMK_DIR/colagan_v2_right_only_via.uf2" "$PROJECT_DIR/firmware.uf2"

echo "✅ Firmware compilé : firmware.uf2 ($(ls -lh "$PROJECT_DIR/firmware.uf2" | awk '{print $5}'))"
echo ""
echo "📌 Pour flasher :"
echo "   1. Mettre le Pico en mode bootloader (BOOT + RESET)"
echo "   2. Copier : cp firmware.uf2 /media/\$USER/RPI-RP2/"
