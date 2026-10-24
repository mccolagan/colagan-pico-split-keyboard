#!/bin/bash
# Compile le firmware de test du lien split (même .uf2 pour les deux moitiés)
# Chemins surchargeables : QMK_DIR (clone de qmk_firmware) et QMK_BIN (CLI qmk)

set -e

PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_DIR="${QMK_DIR:-$PROJECT_DIR/../qmk_firmware}"
QMK_BIN="${QMK_BIN:-$PROJECT_DIR/../qmk_env/bin/qmk}"
KB_DIR="$QMK_DIR/keyboards/colagan_v2_split_test"

mkdir -p "$KB_DIR/keymaps/default"
cp "$PROJECT_DIR/split-test/keyboard.json" "$PROJECT_DIR/split-test/config.h" "$PROJECT_DIR/split-test/rules.mk" "$KB_DIR/"
cp "$PROJECT_DIR/split-test/keymaps/default/keymap.c" "$KB_DIR/keymaps/default/keymap.c"

cd "$QMK_DIR"
"$QMK_BIN" compile -kb colagan_v2_split_test -km default

cp "$QMK_DIR/colagan_v2_split_test_default.uf2" "$PROJECT_DIR/firmware-split-test.uf2"
echo "✅ firmware-split-test.uf2 prêt (à flasher sur les deux moitiés)"
