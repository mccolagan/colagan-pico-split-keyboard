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

- **Configurateur web** (dépôt `colagan-configurator`), page Touches, mode *Couleurs* : couleur de chaque touche
  par couche (ou « Effet » pour laisser voir l'effet RGB), effet propre à une couche, effet global sur la
  couche 0. Écrit immédiatement dans la mémoire du clavier ; pendant l'édition, le clavier affiche la couche
  choisie. Pas besoin de recompiler.
- Vial (onglet *Lighting*) ne règle que l'effet global.
- Valeurs par défaut des couleurs par couche : tableau `ledmaps` de `keymaps/vial/keymap.c` (`__` = effet,
  `OFF` = éteinte, couleurs dans `palette`). Elles ne sont chargées que si la mémoire n'a pas encore de
  couleurs : premier démarrage de ce firmware, ou bootmagic.
- Stockage (`led_layers.c`) : bloc clavier de l'EEPROM, 1 512 octets (8 couches × 58 LED en RGB, masque
  « effet », effet par couche). La moitié branchée en USB fait foi et envoie les couleurs à l'autre par le lien
  split ; brancher la gauche seule utilise **ses** couleurs enregistrées.
- Les couleurs par couche ne sont **pas** dans le `.vil` de Vial : elles survivent aux reflashs, pas à un
  bootmagic.

## Géométrie

`gen-layout.py` régénère `keyboard.json` (LAYOUT, positions des LED) et `vial.json` à partir des positions
des switches du PCB KiCad (pouces en arc, colonne extérieure en 1,5U, moitié gauche en miroir).
Lancé automatiquement par `build-vial.sh`.

## Commande du configurateur (raw HID `0xC0`)

Protocole « LED stream » v2, spécifié dans [`docs/led-stream-protocol.md`](../docs/led-stream-protocol.md) :
lecture en direct de la couleur réelle des 58 LED, lecture / écriture des couleurs et de l'effet de chaque
couche, aperçu d'une couche sur le clavier (expire au bout de 3 s sans renouvellement). Aucun déverrouillage
Vial requis. Test : `tools/led-stream-test.py` (info + une image) ou `--frames 100` (cadence, ~44 i/s avec les
deux moitiés). Les deux moitiés doivent avoir ce firmware.

**Mise à jour depuis un firmware antérieur au 05/11/2026** : l'EEPROM émulée a été agrandie (8 Ko) pour garder
la place des macros ; au premier démarrage, **toute la mémoire repart à zéro** (keymap, effet RGB, réglages).
Exporter le `.vil` avant, le réimporter après, refaire l'effet RGB.
