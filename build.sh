#!/bin/bash
# Script de compilation du firmware Colagan V2 Right Only

set -e  # Arrêter en cas d'erreur

echo "📋 Synchronisation des fichiers keymap..."
cp /home/kebab/perso/clavier/colagan-pico-split-keyboard/keymaps/via/keymap.c \
   /home/kebab/perso/clavier/qmk_firmware/keyboards/colagan_v2_right_only/keymaps/via/keymap.c
cp /home/kebab/perso/clavier/colagan-pico-split-keyboard/keymaps/via/config.h \
   /home/kebab/perso/clavier/qmk_firmware/keyboards/colagan_v2_right_only/keymaps/via/config.h
cp /home/kebab/perso/clavier/colagan-pico-split-keyboard/keyboard.json \
   /home/kebab/perso/clavier/qmk_firmware/keyboards/colagan_v2_right_only/keyboard.json

echo "🔨 Compilation du firmware..."
cd ~/perso/clavier/qmk_firmware
~/perso/clavier/qmk_env/bin/qmk compile -kb colagan_v2_right_only -km via

echo "📦 Copie du firmware dans le projet..."
cp ~/perso/clavier/qmk_firmware/colagan_v2_right_only_via.uf2 /home/kebab/perso/clavier/colagan-pico-split-keyboard/firmware.uf2

echo "✅ Firmware compilé : firmware.uf2 ($(ls -lh /home/kebab/perso/clavier/colagan-pico-split-keyboard/firmware.uf2 | awk '{print $5}'))"
echo ""
echo "📌 Pour flasher :"
echo "   1. Mettre le Pico en mode bootloader (BOOT + RESET)"
echo "   2. Copier : cp firmware.uf2 /media/\$USER/RPI-RP2/"