# ⌨️ Colagan V2 Right Only

> Clavier split custom - moitié droite uniquement, configurable via VIA

---

## ✨ Caractéristiques

- 🧠 **Processeur:** RP2040 (Raspberry Pi Pico)
- ⌨️ **Layout:** 30 touches (matrice 6×5)
- 🔌 **Configuration:** VIA (reconfiguration en temps réel)
- 💡 **RGB:** 30 LEDs SK6812 MINI-E (1 par touche)
- 🖐️ **Ergonomie:** Ligne 5 en forme de "montagne" pour les pouces
- 🇫🇷 **Optimisé:** AZERTY français

---

## 🏔️ Disposition physique

### Ligne 5 - Forme ergonomique pour les pouces

```
       3        ← Sommet (la plus haute)
      / \
     2   4      ← Flancs intermédiaires
    /     \
   1       5    ← Base alignée
```

Cette disposition en forme de montagne offre un accès naturel et ergonomique aux touches du pouce.

---

## 📁 Structure des fichiers

```
colagan-pico-split-keyboard/
├── 🔨 build.sh                       # Script de compilation rapide
├── ⚡ flash.sh                       # Script de flash automatique
├── 📦 firmware.uf2                   # Firmware compilé (versionné)
├── ⚙️ keyboard.json                  # Configuration matérielle QMK + RGB
├── 📖 readme.md                      # Ce fichier
└── 🗂️ keymaps/
    └── via/                          # Keymap avec support VIA
        ├── keymap.c                  # Layout des touches
        ├── config.h                  # Configuration additionnelle
        └── rules.mk                  # Options de compilation
```

---

## 🚀 Quick Start

### 1️⃣ Compiler le firmware

**Méthode rapide (recommandée) :**
```bash
./build.sh
```

**Méthode manuelle :**
```bash
~/qmk_env/bin/qmk compile -kb colagan_v2_right_only -km via
cp ~/qmk_firmware/colagan_v2_right_only_via.uf2 ./firmware.uf2
```

### 2️⃣ Flasher le Pico

**Entrer en mode bootloader :**

- 🔹 **Méthode 1:** Maintenir `BOOT` + appuyer `RESET` + relâcher `BOOT`
- 🔹 **Méthode 2:** Double-tap rapide sur `RESET`

**Flasher automatiquement :**
```bash
./flash.sh
```

**Ou manuellement :**
```bash
# Détection automatique du Pico
sudo cp firmware.uf2 /media/$USER/RPI-RP2*/
```

✅ Le clavier redémarre automatiquement !

### 3️⃣ Configurer avec VIA

1. 🌐 Ouvrez [VIA](https://usevia.app) dans votre navigateur
2. 🔌 Le clavier est détecté automatiquement
3. ✨ Configurez vos touches en temps réel

**💡 Reset EEPROM (appliquer le layout du firmware) :**
- VIA → Settings ⚙️ → Clear EEPROM / Reset EEPROM
- Déconnectez/reconnectez le clavier

---

## 🎹 Configuration VIA

### 🔄 Correspondances AZERTY ↔ QWERTY

> ⚠️ VIA affiche en **QWERTY US**, mais votre OS utilise **AZERTY français**

| 🇫🇷 Pour obtenir (AZERTY) | 🇺🇸 Sélectionner dans VIA |
|---------------------------|---------------------------|
| `'` (apostrophe)          | `4`                       |
| `é`                       | `2`                       |
| `è`                       | `7`                       |
| `à`                       | `0`                       |
| `ù`                       | `'` (quote QWERTY)        |
| `ç`                       | `9`                       |
| `-` (tiret)               | `6`                       |
| `A`                       | `Q`                       |
| `Q`                       | `A`                       |
| `W`                       | `Z`                       |
| `Z`                       | `W`                       |
| `M`                       | `;` (point-virgule)       |
| `,` (virgule)             | `M`                       |

**💡 Astuce :** Basculez temporairement en QWERTY pour voir en direct :

```bash
setxkbmap us    # Passer en QWERTY
setxkbmap fr    # Revenir en AZERTY
```

### 🎚️ Gestion des layers

| Keycode      | Action                                |
|--------------|---------------------------------------|
| `MO(1)`      | 👆 Layer 1 momentanée (maintien)     |
| `TG(1)`      | 🔄 Toggle layer 1 (bascule)           |
| `TO(1)`      | ➡️ Passer définitivement à layer 1    |
| `LT(1, KC)`  | 🔀 Layer 1 en maintien, touche KC en tap |

---

## 🔧 Configuration matérielle

### 📌 Pins GPIO

**Colonnes (6):** `GP20`, `GP22`, `GP12`, `GP11`, `GP10`, `GP9`
**Lignes (5):** `GP21`, `GP19`, `GP18`, `GP17`, `GP16`
**RGB LEDs:** `GP13` (WS2812/SK6812 data)

### 🔌 USB VID/PID

- **Vendor ID:** `0xFEED`
- **Product ID:** `0x0004`
- **Nom:** Colagan V2 Right Only

---

## 💡 RGB Matrix

### 🌈 Contrôles RGB

Les LEDs RGB peuvent être contrôlées avec les keycodes suivants dans VIA (onglet **Lighting** → **QMK RGB Matrix**) :

| Keycode       | Action                                    |
|---------------|-------------------------------------------|
| `RM_TOGG`     | 💡 On/Off des LEDs                        |
| `RM_NEXT`     | ➡️ Animation suivante                     |
| `RM_PREV`     | ⬅️ Animation précédente                   |
| `RM_HUEU`     | 🔴 Augmenter la teinte (hue)              |
| `RM_HUED`     | 🔵 Diminuer la teinte (hue)               |
| `RM_SATU`     | ⬆️ Augmenter la saturation                |
| `RM_SATD`     | ⬇️ Diminuer la saturation                 |
| `RM_VALU`     | ☀️ Augmenter la luminosité                |
| `RM_VALD`     | 🌙 Diminuer la luminosité                 |
| `RM_SPDU`     | ⚡ Augmenter la vitesse d'animation       |
| `RM_SPDD`     | 🐌 Diminuer la vitesse d'animation        |

**💡 Astuce :** Assignez ces keycodes à des touches via VIA pour contrôler les LEDs sans passer par l'interface web.

### ✨ Animations disponibles

Le firmware inclut plus de 40 animations RGB Matrix :

- 🌊 **Ambiance:** Breathing, Gradient, Rainbow Moving Chevron
- 🎨 **Cycle:** Cycle All, Cycle Pinwheel, Cycle Spiral
- 🌧️ **Effets:** Raindrops, Digital Rain, Pixel Rain
- 🔥 **Réactives:** Typing Heatmap, Solid Reactive, Splash
- ⭐ **Avancées:** Dual Beacon, Rainbow Beacon, Jellybean Raindrops

### ⚙️ Configuration RGB

- **LEDs:** 30 SK6812 MINI-E (1 par touche)
- **Câblage:** Serpentin (zigzag ligne par ligne)
- **Luminosité max:** 50 (sur 255)
- **Pin data:** GP13

---

## 🛠️ Dépannage

### ❌ VIA ne détecte pas le clavier

1. ✅ Vérifiez la compilation avec `-km via`
2. 🔄 Rechargez la page VIA
3. 📂 Chargez manuellement `colagan_via_v1.json` dans l'onglet Design
4. 🔌 Vérifiez la connexion USB :
   ```bash
   lsusb | grep -i feed
   # Devrait afficher : ID feed:0004 Colagan V2 Right Only
   ```

### 🔤 Les touches ne correspondent pas

➡️ **Normal !** VIA affiche en QWERTY, votre OS est en AZERTY
📋 Référez-vous au tableau de correspondances ci-dessus

### 💾 Les modifications VIA ne persistent pas

- Les changements sont sauvegardés dans l'EEPROM du clavier
- Vérifiez que `VIA_ENABLE = yes` dans `keymaps/via/rules.mk`

### 🔄 Reset de la configuration VIA

**Option 1 :** Dans VIA → Settings ⚙️ → "Clear EEPROM" ou "Reset EEPROM", puis déconnectez/reconnectez
**Option 2 :** Bootmagic Reset - Maintenez la touche en haut à gauche (L0C0) pendant le branchement USB

### 💡 Les LEDs RGB ne fonctionnent pas

1. ✅ Vérifiez que `rgb_matrix: true` est activé dans `keyboard.json`
2. 🔌 Vérifiez le câblage sur GP13
3. 🔄 Assignez `RM_TOGG` à une touche via VIA pour tester (onglet Lighting → QMK RGB Matrix)
4. 🎨 Vérifiez le câblage serpentin (ligne 0 gauche→droite, ligne 1 droite→gauche, etc.)
5. 💡 Vérifiez que la luminosité n'est pas à 0 (utilisez `RM_VALU` pour augmenter)

---

## 📚 Commandes utiles

```bash
# 🔨 Compiler
~/qmk_env/bin/qmk compile -kb colagan_v2_right_only -km via

# ⚡ Flasher directement
~/qmk_env/bin/qmk flash -kb colagan_v2_right_only -km via

# 🧹 Nettoyer les fichiers de compilation
~/qmk_env/bin/qmk clean

# 🩺 Vérifier la configuration QMK
~/qmk_env/bin/qmk doctor

# ℹ️ Informations du clavier
~/qmk_env/bin/qmk info -kb colagan_v2_right_only
```

---

## 🔗 Ressources

- 🌐 [VIA Web App](https://usevia.app)
- 📖 [Documentation VIA](https://www.caniusevia.com/)
- 📚 [Documentation QMK](https://docs.qmk.fm/)
- 🔧 [QMK VIA Support](https://docs.qmk.fm/features/via)
- 🔌 [Configuration RP2040](https://docs.qmk.fm/platformdev_rp2040)

---

## 📄 License

GPL-2.0-or-later