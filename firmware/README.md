# Firmware Vial — Colagan V2 split

Firmware [Vial](https://get.vial.today/) des deux moitiés (RP2040). La moitié branchée au PC devient maître
(détection USB, 5 s max ; éviter les ports « clavier » des KVM) et l'autre lui envoie ses touches par le câble USB-C. La keymap Vial est enregistrée dans
le maître : **brancher la droite** (Pico classique) au quotidien ; la gauche (Pico W) a sa propre mémoire.

## Compiler et flasher

```bash
./build-vial.sh both          # ou left / right → firmware-vial-left.uf2, firmware-vial-right.uf2
cp firmware-vial-right.uf2 /run/media/$USER/RPI-RP2/
```

Les deux moitiés doivent tourner sur la même version du firmware (lien série, couleurs par couche).

Passer une moitié en bootloader, branchée **seule** au PC :

| Moitié | Moyen |
|---|---|
| Droite | `QK_BOOT` (3 appuis rapides sur le pouce droit extérieur, puis touche en bas à droite), ou bouton BOOTSEL |
| Les deux | Bootmagic : tenir la touche extérieure du haut (Échap à gauche, `)` à droite) en branchant. **Efface la mémoire** (keymap Vial remise à zéro sur la droite) |

## Modifier les touches

Ouvrir Vial (application ou [vial.rocks](https://vial.rocks)) avec la droite branchée : le clavier est reconnu
sans fichier à charger. Les changements sont immédiats et gardés dans la mémoire de la droite.
8 couches, macros, tap-dance, combos. Déverrouillage (si Vial le demande) : tenir les deux touches 1,5U
extérieures de la droite, en haut et en bas.

`keymaps/vial/keymap.c` ne définit que la keymap **par défaut** (premier démarrage ou « Reset » dans Vial).
**Chaque reflash remet cette keymap par défaut** (Vial change d'identifiant à chaque compilation) : sauvegarder
avant (*File → Save current layout*, `.vil`) et recharger après (*Load*).

Changer de couche : 3 appuis en moins de 500 ms sur le pouce droit extérieur (touche `SEL x3`, onglet *User*
de Vial) ouvrent le sélecteur ; 1re / 2e / 3e touche du haut à droite = BASE / FPS / MMO. Un nouvel appui sur
le pouce le referme sans changer de couche.

## Modifier les LED

- Effet, couleur, luminosité, vitesse : onglet *Lighting* de Vial.
- Couleurs par couche : tableau `ledmaps` de `keymaps/vial/keymap.c`, même disposition que les keymaps
  (`__` = effet Vial, `OFF` = éteinte, couleurs dans `palette`). Recompiler et flasher les deux moitiés.

## Géométrie

`gen-layout.py` régénère `keyboard.json` (LAYOUT, positions des LED) et `vial.json` à partir des positions
des switches du PCB KiCad (pouces en arc, colonne extérieure en 1,5U, moitié gauche en miroir).
Lancé automatiquement par `build-vial.sh`.
