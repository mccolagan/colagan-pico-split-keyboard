#!/bin/bash
# Compile le firmware de test split. Usage : ./build-split-test.sh [bench|left|leftr|leftled|right]
#   bench (défaut) : Pico libres reliés par un fil sur GP26, même .uf2 sur les deux
#   leftr         : moitié gauche (rôle gauche) avec le brochage complet de la droite (matrice et LED GP13)
#   left / right   : brochage réel d'une moitié du PCB V2 (un .uf2 par moitié)
# Chemins surchargeables : QMK_DIR (clone de qmk_firmware) et QMK_BIN (CLI qmk)

set -e

TARGET="${1:-bench}"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
QMK_DIR="${QMK_DIR:-$PROJECT_DIR/../qmk_firmware}"
QMK_BIN="${QMK_BIN:-$PROJECT_DIR/../qmk_env/bin/qmk}"
KB_DIR="$QMK_DIR/keyboards/colagan_v2_split_test"

case "$TARGET" in
    bench) FLAGS=""; OUT="firmware-split-test.uf2" ;;
    left)  FLAGS="-DHALF_LEFT"; OUT="firmware-split-left.uf2" ;;
    leftled) FLAGS="-DHALF_LEFT -DRGB_TEST_FIRST_LEDS"; OUT="firmware-split-left-ledtest.uf2" ;;
    leftr) FLAGS="-DHALF_LEFT -DHALF_LEFT_RIGHTPINS"; OUT="firmware-split-left-rightpins.uf2" ;;
    right) FLAGS="-DHALF_RIGHT"; OUT="firmware-split-right.uf2" ;;
    *) echo "Usage : $0 [bench|left|leftr|leftled|right]"; exit 1 ;;
esac

mkdir -p "$KB_DIR/keymaps/default"
cp "$PROJECT_DIR/split-test/keyboard.json" "$PROJECT_DIR/split-test/config.h" "$PROJECT_DIR/split-test/rules.mk" "$KB_DIR/"
cp "$PROJECT_DIR/split-test/keymaps/default/keymap.c" "$KB_DIR/keymaps/default/keymap.c"

cd "$QMK_DIR"
"$QMK_BIN" clean -a >/dev/null 2>&1 || true
"$QMK_BIN" compile -kb colagan_v2_split_test -km default ${FLAGS:+-e EXTRAFLAGS="$FLAGS"}

cp "$QMK_DIR/colagan_v2_split_test_default.uf2" "$PROJECT_DIR/$OUT"
echo "✅ $OUT prêt"
