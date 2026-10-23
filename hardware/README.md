# Matériel — Colagan Pico Split KeyBoard V.1

Sources KiCad du PCB réversible (29 touches KS33, Raspberry Pi Pico, USB-C, LEDs adressables par touche).

| Chemin | Contenu |
|---|---|
| `kicad/SPlitKeyBoardReversible.*` | Projet principal : PCB, schéma, projet (20/10/2025) |
| `kicad/SPlitKeyBoardReversibleOLED.*` | Variante avec écran OLED |
| `kicad/Library*.pretty`, `Colagan 1.5U.kicad_sym` | Empreintes et symbole personnalisés (touche 1U/1,5U réversible, Pico) |
| `images/` | Captures KiCad : vues du PCB (cuivre, 3D), schémas |
| `3d/` | Export STEP / 3MF de la version V1 |

## Notes
- La chaîne de LEDs est linéaire : LED n = touche CH(n+1), de gauche à droite sur chaque rangée.
- Pouces : CH25 à CH29 en arc, rotations 45° / 22,5° / 0° / −22,5° / −45°.
- Non versionnés (volumineux) : les exports STEP de 5,5 et 7 Mo, le modèle 3D du switch et les Gerbers de test.
