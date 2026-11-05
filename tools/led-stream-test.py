#!/usr/bin/env python3
"""Lit l'état réel des LED du Colagan V2 par raw HID (commande 0xC0, voir docs/led-stream-protocol.md).

Usage : tools/led-stream-test.py [--frames N]
  sans option : info + positions + une image, affichées en couleur dans le terminal
  --frames N  : lit N images d'affilée et affiche la cadence obtenue

Python standard uniquement. Cherche le /dev/hidraw* de VID:PID FEED:0004 dont le descripteur déclare la
page d'usage 0xFF60 (interface raw HID de VIA/Vial). Ne touche à aucun autre périphérique.
"""
import argparse
import glob
import os
import select
import sys
import time

VID, PID = 0xFEED, 0x0004
CMD = 0xC0
MARKER = 0xFF
SUB_INFO, SUB_POSITIONS, SUB_COLORS = 0x00, 0x01, 0x02
STATUS = {
    0x00: "OK",
    0x01: "sous-commande inconnue",
    0x02: "start hors bornes",
    0x03: "requête mal formée",
    0x10: "autre moitié injoignable",
    0x11: "autre moitié sans ce firmware",
    0xFF: "écho brut (déverrouillage Vial en cours ?)",
}


def find_device():
    for node in sorted(glob.glob("/sys/class/hidraw/hidraw*")):
        try:
            with open(f"{node}/device/uevent") as f:
                uevent = f.read()
            with open(f"{node}/device/report_descriptor", "rb") as f:
                descriptor = f.read()
        except OSError:
            continue
        if f"HID_ID=0003:{VID:08X}:{PID:08X}" in uevent and b"\x06\x60\xff" in descriptor:
            return "/dev/" + os.path.basename(node)
    return None


class Keyboard:
    def __init__(self, path):
        self.fd = os.open(path, os.O_RDWR)

    def request(self, sub, start=0):
        packet = bytes([CMD, sub, MARKER, start]).ljust(32, b"\x00")
        os.write(self.fd, b"\x00" + packet)  # octet 0 = numéro de rapport (aucun)
        while True:
            ready, _, _ = select.select([self.fd], [], [], 1.0)
            if not ready:
                raise TimeoutError("pas de réponse du clavier")
            resp = os.read(self.fd, 32)
            if resp[0] == 0xFF and resp[1] == sub:
                raise RuntimeError("commande 0xC0 non gérée : firmware sans led_stream")
            if resp[0] == CMD and resp[1] == sub:
                return resp


def check(resp, fatal=True):
    status = resp[2]
    if status != 0 and fatal and status < 0x10:
        raise RuntimeError(f"statut 0x{status:02X} : {STATUS.get(status, '?')}")
    return status


def read_info(kb):
    r = kb.request(SUB_INFO)
    check(r)
    d = r[3:]
    return {
        "version": d[0], "led_count": d[1], "colors_per_packet": d[2], "positions_per_packet": d[3],
        "left_count": d[4], "master": "droite" if d[5] else "gauche", "flags": d[6],
        "max_brightness": d[7], "val": d[8], "rows": d[9], "cols": d[10],
    }


def read_positions(kb, info):
    positions = []
    while len(positions) < info["led_count"]:
        r = kb.request(SUB_POSITIONS, len(positions))
        check(r)
        positions += [(r[5 + 2 * i], r[6 + 2 * i]) for i in range(r[4])]
    return positions


def read_frame(kb, info):
    colors, statuses = [], set()
    while len(colors) < info["led_count"]:
        r = kb.request(SUB_COLORS, len(colors))
        statuses.add(check(r, fatal=False))
        colors += [tuple(r[5 + 3 * i:8 + 3 * i]) for i in range(r[4])]
    return colors, statuses


def cie(v):
    """Courbe CIE 1931 de QMK (quantum/led_tables.c) : valeur PWM maximale d'un effet à la luminosité v."""
    lightness = v * 100 / 255
    y = lightness / 902.3 if lightness <= 8 else ((lightness + 16) / 116) ** 3
    return -int(-255 * y // 1)


def swatch(rgb, ref):
    m = max(rgb)
    k = min(255 / ref, 255 / m) if m and ref else 0
    r, g, b = (round(c * k) for c in rgb)
    return f"\x1b[48;2;{r};{g};{b}m    \x1b[0m"


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--frames", type=int, default=0)
    args = parser.parse_args()

    path = find_device()
    if not path:
        sys.exit("Colagan (FEED:0004, interface 0xFF60) introuvable. Branché ? Droits sur /dev/hidraw* (règle udev) ?")
    kb = Keyboard(path)
    info = read_info(kb)
    flags = info["flags"]
    print(f"{path} : protocole v{info['version']}, {info['led_count']} LED, maître {info['master']}, "
          f"luminosité {info['val']}/{info['max_brightness']}, matrice {info['rows']}×{info['cols']}")
    print(f"  split RPC={bool(flags & 1)}  autre moitié connectée={bool(flags & 2)}  RGB allumé={bool(flags & 4)}")

    if args.frames:
        start = time.monotonic()
        for _ in range(args.frames):
            _, statuses = read_frame(kb, info)
        elapsed = time.monotonic() - start
        print(f"{args.frames} images en {elapsed:.2f} s → {args.frames / elapsed:.1f} i/s, statuts {sorted(statuses)}")
        return

    positions = read_positions(kb, info)
    colors, statuses = read_frame(kb, info)
    for s in sorted(statuses):
        if s:
            print(f"  ⚠ statut 0x{s:02X} : {STATUS.get(s, '?')}")

    # Affichage par position matricielle, normalisé comme conseillé dans la spec (section « Valeurs »)
    scale = cie(info["val"])
    grid = {pos: (led, rgb) for led, (pos, rgb) in enumerate(zip(positions, colors))}
    for row in range(info["rows"]):
        line = []
        for col in range(info["cols"]):
            led, rgb = grid.get((row, col), (None, None))
            line.append("      " if led is None else swatch(rgb, scale) + "  ")
        side = "G" if row < info["rows"] // 2 else "D"
        print(f"  {side} ligne {row} : " + "".join(line))
    print()
    for led, ((row, col), rgb) in enumerate(zip(positions, colors)):
        print(f"  LED {led:2d} [{row},{col}] CH{led + 1:<2d} RGB {rgb[0]:3d} {rgb[1]:3d} {rgb[2]:3d} {swatch(rgb, scale)}")


if __name__ == "__main__":
    try:
        main()
    except (RuntimeError, TimeoutError, PermissionError) as e:
        sys.exit(f"Erreur : {e}")
