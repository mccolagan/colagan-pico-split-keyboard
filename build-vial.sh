#!/bin/bash
# Compile le firmware Vial du Colagan V2 split. Usage : ./build-vial.sh [left|right|both]
#   right (défaut) : moitié droite (Pico classique, à brancher au PC : le maître garde la keymap Vial)
#   left           : moitié gauche (Pico W monté face arrière)
# Sources : firmware/colagan_v2 (copiées dans vial-qmk/keyboards/colagan_v2 à chaque build).
# Géométrie (keyboard.json, vial.json) : régénérée par firmware/gen-layout.py.
# Chemins surchargeables : VIAL_DIR (clone de vial-qmk) et QMK_BIN (CLI qmk)

set -e

TARGET="${1:-right}"
PROJECT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
VIAL_DIR="${VIAL_DIR:-$PROJECT_DIR/../vial-qmk}"
QMK_BIN="${QMK_BIN:-$PROJECT_DIR/../qmk_env/bin/qmk}"
KB_DIR="$VIAL_DIR/keyboards/colagan_v2"

build() {
    local half="$1" flag
    case "$half" in
        left)  flag="-DHALF_LEFT" ;;
        right) flag="-DHALF_RIGHT" ;;
        *) echo "Usage : $0 [left|right|both]"; exit 1 ;;
    esac

    rm -rf "$KB_DIR"
    cp -r "$PROJECT_DIR/firmware/colagan_v2" "$KB_DIR"

    (cd "$VIAL_DIR" && QMK_HOME="$VIAL_DIR" "$QMK_BIN" clean >/dev/null 2>&1 || true)
    (cd "$VIAL_DIR" && QMK_HOME="$VIAL_DIR" "$QMK_BIN" compile -kb colagan_v2 -km vial -e EXTRAFLAGS="$flag")

    cp "$VIAL_DIR/colagan_v2_vial.uf2" "$PROJECT_DIR/firmware-vial-$half.uf2"
    echo "✅ firmware-vial-$half.uf2 prêt"
}

python3 "$PROJECT_DIR/firmware/gen-layout.py"
if [ "$TARGET" = both ]; then
    build right
    build left
else
    build "$TARGET"
fi
