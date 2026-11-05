#!/usr/bin/env python3
"""Génère la géométrie du Colagan V2 split à partir des positions KiCad du PCB fabriqué.

Écrit :
  - colagan_v2/keyboard.json        (layouts.LAYOUT + rgb_matrix.layout)
  - colagan_v2/keymaps/vial/vial.json (disposition affichée dans Vial)

Matrice : [r, c] = switch CH(6r + c + 1) d'une moitié ; moitié gauche = lignes 0-4, droite = lignes 5-9.
LED : chaîne linéaire, LED n d'une moitié = CH(n + 1) ; LED 0-28 gauche, 29-57 droite.
Ordre du LAYOUT (et des keymaps) : rangée par rangée, de gauche à droite à l'écran ; puis les pouces.
"""

import json
from pathlib import Path

U = 19.05
# Centres des switches CH1..CH29 (mm, PCB face avant = moitié droite) et rotation KiCad (degrés, sens trigo)
KICAD = {
    **{6 * r + c + 1: (168.939626 + c * U + (0.25 * U if c == 5 else 0), 97.446176 + r * U, 0)
       for r in range(4) for c in range(6)},
    25: (128.363955, 190.5, 45),
    26: (146.964517, 178.029275, 22.5),
    27: (168.939626, 173.646176, 0),
    28: (190.916662, 178.029275, -22.5),
    29: (209.55, 190.5, -45),
}

THUMB_GAP = 2.0   # écart (U) entre les centres des pouces les plus intérieurs des deux moitiés
LEFT_X = 6.0      # décalage de la colonne 0 de la moitié gauche (bord gauche du clavier à x = 0)


def local(ch):
    """Centre (U) relatif à CH1 de la moitié droite, rotation au sens KLE (horaire)."""
    x, y, rot = KICAD[ch]
    return (x - KICAD[1][0]) / U, (y - KICAD[1][1]) / U, -rot


def width(ch):
    return 1.5 if ch <= 24 and (ch - 1) % 6 == 5 else 1.0


RIGHT_X = LEFT_X + 2 * -local(25)[0] + THUMB_GAP


def key(half, ch):
    """Touche placée dans le plan global : la moitié gauche est le miroir de la droite."""
    lx, ly, rot = local(ch)
    if half == 'left':
        cx, rot = LEFT_X - lx, -rot
    else:
        cx = RIGHT_X + lx
    row0 = 0 if half == 'left' else 5
    r, c = (ch - 1) // 6, (ch - 1) % 6
    return {
        'matrix': [row0 + r, c],
        'cx': round(cx, 4), 'cy': round(ly + 0.5, 4), 'w': width(ch), 'rot': round(rot, 4), 'ch': ch,
    }


def ordered_keys():
    keys = []
    for r in range(4):
        keys += [key('left', 6 * r + c + 1) for c in reversed(range(6))]
        keys += [key('right', 6 * r + c + 1) for c in range(6)]
    keys += [key('left', ch) for ch in (29, 28, 27, 26, 25)]
    keys += [key('right', ch) for ch in (25, 26, 27, 28, 29)]
    return keys


def qmk_layout(keys):
    out = []
    for k in keys:
        e = {'matrix': k['matrix'], 'x': round(k['cx'] - k['w'] / 2, 4), 'y': round(k['cy'] - 0.5, 4)}
        if k['w'] != 1:
            e['w'] = k['w']
        if k['rot']:
            e.update({'r': k['rot'], 'rx': k['cx'], 'ry': k['cy']})
        out.append(e)
    return out


def rgb_layout(keys):
    by_matrix = {tuple(k['matrix']): k for k in keys}
    xs = [k['cx'] for k in keys]
    ys = [k['cy'] for k in keys]
    leds = []
    for row0 in (0, 5):
        for n in range(29):
            k = by_matrix[(row0 + n // 6, n % 6)]
            leds.append({
                'matrix': k['matrix'],
                'x': round(224 * (k['cx'] - min(xs)) / (max(xs) - min(xs))),
                'y': round(64 * (k['cy'] - min(ys)) / (max(ys) - min(ys))),
                'flags': 4,
            })
    return leds


def kle(keys):
    rows = []
    grid = [k for k in keys if k['ch'] <= 24]
    for r in range(4):
        row, x = [], 0.0
        for k in grid[12 * r:12 * (r + 1)]:
            left = k['cx'] - k['w'] / 2
            props = {}
            if abs(left - x) > 1e-4:
                props['x'] = round(left - x, 4)
            if k['w'] != 1:
                props['w'] = k['w']
            if props:
                row.append(props)
            row.append(f"{k['matrix'][0]},{k['matrix'][1]}")
            x = left + k['w']
        rows.append(row)
    for k in keys[48:]:
        rows.append([{'r': k['rot'], 'rx': k['cx'], 'ry': k['cy'], 'x': -0.5, 'y': -0.5},
                     f"{k['matrix'][0]},{k['matrix'][1]}"])
    return rows


def main():
    base = Path(__file__).resolve().parent / 'colagan_v2'
    keys = ordered_keys()

    kb_path = base / 'keyboard.json'
    kb = json.loads(kb_path.read_text())
    kb['layouts'] = {'LAYOUT': {'layout': qmk_layout(keys)}}
    kb['rgb_matrix']['layout'] = rgb_layout(keys)
    kb_path.write_text(json.dumps(kb, indent=4, ensure_ascii=False) + '\n')

    vial_path = base / 'keymaps' / 'vial' / 'vial.json'
    vial = json.loads(vial_path.read_text())
    vial['layouts'] = {'keymap': kle(keys)}
    vial_path.write_text(json.dumps(vial, indent=4, ensure_ascii=False) + '\n')

    print(f'{len(keys)} touches, {len(kb["rgb_matrix"]["layout"])} LED -> {kb_path.name}, {vial_path.name}')


if __name__ == '__main__':
    main()
