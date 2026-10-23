# Matériel — Colagan Pico Split KeyBoard V.1

Sources KiCad (9.0) du PCB réversible : 29 touches Gateron KS-33 low profile en hot-swap, une LED SK6812 mini-e et une diode par touche, Raspberry Pi Pico, USB-C. Le même circuit sert pour les deux moitiés, en le retournant.

![Rendu 3D du PCB](images/Capture%20d_%C3%A9cran%20Globale%203D.png)

## Ouvrir le projet

Ouvrir `kicad/SPlitKeyBoardReversible.kicad_pro` avec KiCad 9. Les fichiers `fp-lib-table` et `sym-lib-table` du dossier déclarent les bibliothèques du projet (`Library SplitKeyboard Colagan` et `Colagan`), donc rien à configurer. Les bibliothèques standard de KiCad (`Connector`, `MCU_Module_RaspberryPi_Pico`, `power`, `MountingHole`) viennent de l'installation.

| Fichier | Contenu |
|---|---|
| `kicad/SPlitKeyBoardReversible.kicad_{pro,sch,pcb}` | Projet principal (PCB du 20/10/2025) |
| `kicad/SPlitKeyBoardReversibleOLED.kicad_{pro,pcb}` | Variante avec écran OLED SSD1306 128×32 |
| `kicad/Library SplitKeyboard Colagan.pretty/` | Toutes les empreintes personnalisées (voir ci-dessous) |
| `kicad/Colagan.kicad_sym` | Symboles KS33 hot-swap 1U et 1,5U, extraits du schéma |
| `kicad/Colagan 1.5U.kicad_sym` | Ancienne version du symbole 1,5U |
| `kicad/fp-lib-table`, `kicad/sym-lib-table` | Tables de bibliothèques du projet |
| `kicad/fabrication-toolkit-options.json` | Options du plugin de fabrication |
| `images/` | Captures KiCad : cuivre avant/arrière, 3D, schémas, zones USB-C et conversion de niveau |
| `3d/` | Export STEP et 3MF de la V1 |

## Empreintes (`Library SplitKeyboard Colagan.pretty`)

### Utilisées dans le PCB

| Empreinte | Rôle | Quantité |
|---|---|---|
| `KS33-HS-Colagan-1U-Reverse-REF4` | Touche 1U : interrupteur KS33 hot-swap, LED SK6812 mini-e, diode ; réversible (deux faces) | 25 |
| `KS33-HS-Colagan-1.5U-Reverse-REF4` | Même chose pour la colonne extérieure 1,5U | 4 |
| `RaspberryPi_Pico_Colagan_REF3` | Support du Raspberry Pi Pico | 1 |
| `USB_C_6P_Colagan` | Connecteur USB-C 6 broches (alimentation) | 1 |
| `qrcode github` | QR code du projet, en sérigraphie/cuivre | 1 |
| `OLED_SSD1306_128x32_4pin` | Écran OLED (variante OLED uniquement) | 1 |

La touche KS33 intègre trois composants en un seul placement : c'est une empreinte combinée, pas une empreinte d'interrupteur seule. Broches : 1 colonne, 2 et 3 ligne (via la diode), 4 +5V, 5 DIN de la LED, 6 GND, 7 DOUT de la LED.

### Versions intermédiaires et illustrations

- `KS33-HS-Colagan-1U-Reverse`, `-REF`, `-REF2`, `-REF3`, `-DiodeV2` et `KS33-HS-Colagan-1.5U-Reverse`, `-REF` : itérations précédentes de la touche.
- `RaspberryPi_Pico_Colagan`, `_REF`, `_REF2` : itérations précédentes du support Pico.
- `Gengar*`, `Gengar_Souls.kicad_sym`, `fantominus*`, `spectrum 35mm` : illustrations de sérigraphie / cuivre (empreintes « LOGO »).

## Symboles

`Colagan.kicad_sym` contient `KS33_SW_HS_Colagan_V2_Fix2` (touche 1U) et `KS33_SW_HS_Colagan_V2_1.5U`. Ils étaient embarqués dans le schéma ; ils ont été extraits tels quels dans une bibliothèque pour pouvoir les réutiliser.

## Modèles 3D

Les empreintes référencent des modèles 3D qui **ne sont pas dans ce dépôt** :

- le modèle Gateron KS-33 low profile (`GateronKS-33LowProfile-3D/*.stp`, fourni par le fabricant) ;
- le modèle du switch de la bibliothèque `marbastlib` d'ebastler (installée via le gestionnaire de plugins de KiCad) ;
- un modèle `SK6812-E.step` stocké localement ;
- les modèles du Pico de la bibliothèque standard de KiCad.

Les empreintes fonctionnent sans eux ; seul l'affichage 3D dans KiCad en dépend. Le dossier `3d/` contient un export complet de la carte.

## Électronique (d'après les captures et le PCB)

- **Matrice** : colonnes `Col0`–`Col5`, rangées `Row0`–`Row4`, diodes sur les touches. Brochage du Pico : colonnes GP20, GP22, GP12, GP11, GP10, GP9 ; rangées GP21, GP19, GP18, GP17, GP16 ; LEDs GP13.
- **LEDs** : chaîne linéaire, LED n = touche CH(n+1) (CH1 à CH29, de gauche à droite sur chaque rangée). Alimentées en 5 V.
- **Signal LED** : un composant U3 (SOT) convertit la ligne de données 3,3 V du Pico vers 5 V ; une résistance de 4,7 kΩ (R3) est associée au niveau 5 V (voir `images/Capture Signal led 3.3v to 5v.png`).
- **USB-C** : prise 6 broches en alimentation seule, avec deux résistances de 5,1 kΩ sur CC1/CC2. Les broches de données de la prise sont reliées, par des résistances de 4,7 kΩ (R1, R2), aux signaux `SCL` et `SDA` (voir `images/Capture Type-C.png`).
- **Géométrie** : grille 6×4 au pas de 19,05 mm ; colonne extérieure 1,5U ; cinq pouces CH25–CH29 en arc (rotations 45°, 22,5°, 0°, −22,5°, −45° dans KiCad).

Ces détails sont lus sur le PCB et les captures : à confirmer sur le schéma avant toute réutilisation.

## Non inclus

Les exports STEP lourds (5,5 et 7 Mo), les modèles 3D du switch, les Gerbers des essais précédents (`Test_Split_Reversible`) et les sauvegardes automatiques de KiCad.

## Licence

Aucune licence matérielle n'est encore déclarée pour ces fichiers.
