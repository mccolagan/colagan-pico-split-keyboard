# Colagan V2 Right Only

Clavier split custom - moitié droite uniquement, configurable via VIA.

## Caractéristiques

- **Processeur:** RP2040
- **Layout:** 30 touches (6x5)
- **Matrice:** 5 lignes × 6 colonnes
- **Configuration:** VIA (reconfiguration en temps réel sans recompilation)
- **Disposition physique:** Ligne 5 en forme de "montagne" ergonomique

## Disposition physique

### Ligne 5 - Forme ergonomique
```
       3        ← Sommet (la plus haute)
      / \
     2   4      ← Ligne du milieu
    /     \
   1       5    ← Toutes alignées à la base
```

Cette disposition en forme de montagne offre un accès ergonomique aux touches du pouce.

## Configuration avec VIA

### 1. Accéder à VIA

Ouvrez VIA dans votre navigateur : **https://usevia.app**

Ou installez l'application desktop depuis : https://github.com/the-via/releases/releases

### 2. Charger la définition du clavier

1. Dans VIA, allez dans **Settings** → **Show Design tab**
2. Cliquez sur l'onglet **Design**
3. Cliquez sur **Load** et sélectionnez le fichier `colagan_via_v1.json`
4. Le clavier devrait maintenant être reconnu

### 3. Configurer les touches

- Les modifications sont **instantanées** - pas besoin de recompiler!
- Changez les touches en cliquant dessus et en sélectionnant la nouvelle fonction
- Les changements sont sauvegardés dans la mémoire EEPROM du clavier

### 4. Important - Correspondance AZERTY/QWERTY

VIA affiche les touches en **QWERTY US**, mais votre OS utilise **AZERTY français**.

**Correspondances importantes :**

| Pour obtenir (AZERTY) | Sélectionner dans VIA |
|-----------------------|----------------------|
| ' (apostrophe) | 4 |
| é | 2 |
| è | 7 |
| à | 0 |
| ù | ' (QWERTY) |
| ç | 9 |
| - | 6 |
| A | Q |
| Q | A |
| W | Z |
| Z | W |
| M | ; (point-virgule) |
| , (virgule) | M |

**Astuce :** Basculez temporairement votre OS en QWERTY pendant la configuration pour avoir un aperçu direct.

```bash
# Linux - Basculer temporairement en QWERTY
setxkbmap us

# Revenir en AZERTY français
setxkbmap fr
```

### 5. Gestion des layers

Pour passer à une autre layer dans VIA, utilisez ces keycodes :

- **MO(1)** - Layer 1 momentanée (maintien)
- **TG(1)** - Toggle layer 1 (bascule)
- **TO(1)** - Passer définitivement à layer 1
- **LT(1, KC)** - Layer 1 en maintien, touche KC en tap

## Structure des fichiers

```
keyboards/colagan_v2_right_only/
├── colagan_via_v1.json              # Définition VIA (à charger dans VIA)
├── via_backup.json                  # Backup de votre configuration VIA
├── keyboard.json                     # Configuration matérielle QMK
├── rules.mk                          # Options de compilation
├── readme.md                         # Ce fichier
└── keymaps/
    └── via/                          # Keymap avec support VIA
        ├── keymap.c
        ├── config.h
        └── rules.mk
```

## Compilation et flash

### 1. Compiler le firmware avec support VIA

**Méthode rapide avec le script :**
```bash
./build.sh
```

**Méthode manuelle :**
```bash
# Avec QMK CLI
~/qmk_env/bin/qmk compile -kb colagan_v2_right_only -km via

# Copier dans le projet
cp ~/qmk_firmware/colagan_v2_right_only_via.uf2 ./firmware.uf2
```

Le firmware compilé est versionné dans le repo : `firmware.uf2`

### 2. Flasher sur le RP2040

**Méthode 1 - Mode bootloader :**
1. Maintenir le bouton **BOOT**
2. Appuyer brièvement sur **RESET**
3. Relâcher **BOOT**

**Méthode 2 - Double-tap RESET :**
- Double-tap rapide sur le bouton RESET

**Copier le firmware :**
```bash
# Le RP2040 apparaît comme lecteur USB "RPI-RP2"
sudo cp firmware.uf2 /media/$USER/RPI-RP21/
```

Le clavier redémarre automatiquement avec le nouveau firmware.

### 3. Vérifier que VIA fonctionne

1. Ouvrez VIA (https://usevia.app)
2. Chargez la définition `colagan_via_v1.json`
3. Votre clavier devrait être détecté automatiquement
4. Vous pouvez maintenant configurer vos touches!

## Configuration matérielle

### Pins GPIO (définis dans keyboard.json)

- **Colonnes (cols):** GP20, GP22, GP12, GP11, GP10, GP9
- **Lignes (rows):** GP21, GP19, GP18, GP17, GP16

### USB VID/PID

- **Vendor ID:** 0xFEED
- **Product ID:** 0x0004
- **Nom:** Colagan V2 Right Only

## Dépannage

### VIA ne détecte pas le clavier

1. Vérifiez que vous avez compilé avec `-km via`
2. Vérifiez que le firmware est bien flashé
3. Rechargez la page VIA
4. Essayez de charger manuellement `colagan_via_v1.json` dans l'onglet Design

### Les touches ne correspondent pas aux symboles

- C'est normal! VIA affiche en QWERTY, votre OS est en AZERTY
- Référez-vous au tableau de correspondances ci-dessus
- Ou basculez temporairement en QWERTY pour configurer

### Les modifications VIA ne persistent pas

- Les modifications sont normalement sauvegardées dans l'EEPROM
- Si elles ne persistent pas, essayez de recompiler avec `VIA_ENABLE = yes` dans `rules.mk`

### Le clavier n'est pas reconnu

```bash
# Vérifier que le clavier est connecté
lsusb | grep -i feed

# Devrait afficher quelque chose comme :
# Bus XXX Device XXX: ID feed:0004 Colagan V2 Right Only
```

### Reset de la configuration VIA

Pour remettre le clavier à zéro :
1. Dans VIA, allez dans Settings
2. Cliquez sur "Clear Saved Layout"
3. Ou reflashez le firmware

## Commandes utiles

```bash
# Compiler avec support VIA
~/qmk_env/bin/qmk compile -kb colagan_v2_right_only -km via

# Flasher directement
~/qmk_env/bin/qmk flash -kb colagan_v2_right_only -km via

# Nettoyer les fichiers de compilation
~/qmk_env/bin/qmk clean

# Vérifier la configuration QMK
~/qmk_env/bin/qmk doctor

# Voir les informations du clavier
~/qmk_env/bin/qmk info -kb colagan_v2_right_only
```

## Ressources

- [VIA](https://www.caniusevia.com/) - Site officiel VIA
- [VIA Web App](https://usevia.app) - Application web VIA
- [Documentation QMK](https://docs.qmk.fm/)
- [QMK VIA Support](https://docs.qmk.fm/features/via)
- [Configuration RP2040](https://docs.qmk.fm/platformdev_rp2040)

## License

GPL-2.0-or-later
