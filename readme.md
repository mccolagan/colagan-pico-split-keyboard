# ⌨️ Colagan V2 Right Only

> Clavier split custom - moitié droite uniquement avec layout AZERTY français

---

## ✨ Caractéristiques

- 🧠 **Processeur:** RP2040 (Raspberry Pi Pico)
- ⌨️ **Layout:** 30 touches (matrice 6×5)
- 🔌 **Configuration:** VIA (reconfiguration en temps réel)
- 💡 **RGB:** 30 LEDs SK6812 MINI-E (1 par touche) avec animations personnalisées
- 🖐️ **Ergonomie:** Ligne 5 en forme de "montagne" pour les pouces
- 🇫🇷 **Optimisé:** AZERTY français

---

## 🎮 Layers disponibles

Le clavier dispose de **3 layers** accessibles via une touche multi-tap :

| Taps | Layer | Description |
|------|-------|-------------|
| **1x** | - | Ctrl normal |
| **2x** | **FN** | Côté gauche du clavier AZERTY (toggle) |
| **3x** | **GAMING** | Layout optimisé pour le jeu avec RGB personnalisé (toggle) |

### 🎯 Layer BASE - Côté DROIT AZERTY

```
┌─────┬─────┬─────┬─────┬─────┬─────┐
│  -  │  è  │  _  │  ç  │  à  │  )  │
├─────┼─────┼─────┼─────┼─────┼─────┤
│  Y  │  U  │  I  │  O  │  P  │  ^  │
├─────┼─────┼─────┼─────┼─────┼─────┤
│  H  │  J  │  K  │  L  │  M  │  ù  │
├─────┼─────┼─────┼─────┼─────┼─────┤
│  B  │  N  │  ,  │  ;  │  :  │  !  │
├─────┼─────┼─────┼─────┼─────┼─────┤
│ Del │Bksp │ Ent │ Alt │Ctrl │     │
│     │     │     │     │ 2xFN│     │
└─────┴─────┴─────┴─────┴─────┴─────┘
```

### 🔤 Layer FN - Côté GAUCHE AZERTY

Double-tap sur Ctrl pour activer/désactiver

```
┌─────┬─────┬─────┬─────┬─────┬─────┐
│  ²  │  &  │  é  │  "  │  '  │  (  │
├─────┼─────┼─────┼─────┼─────┼─────┤
│ Tab │  A  │  Z  │  E  │  R  │  T  │
├─────┼─────┼─────┼─────┼─────┼─────┤
│Caps │  Q  │  S  │  D  │  F  │  G  │
├─────┼─────┼─────┼─────┼─────┼─────┤
│Shift│  <  │  W  │  X  │  C  │  V  │
├─────┼─────┼─────┼─────┼─────┼─────┤
│ Del │Bksp │ Ent │ Alt │Ctrl │     │
└─────┴─────┴─────┴─────┴─────┴─────┘
```

### 🎮 Layer GAMING - Layout optimisé pour le jeu

Triple-tap sur Ctrl pour activer/désactiver

```
┌─────┬─────┬─────┬─────┬─────┬─────┐
│  &  │  é  │  "  │  '  │  (  │     │ 🔵🔵🟣🟣🟣
├─────┼─────┼─────┼─────┼─────┼─────┤
│Ctrl │Shift│  Z  │  E  │     │     │ 🟡🟡⚪🟡⚫⚫
├─────┼─────┼─────┼─────┼─────┼─────┤
│     │  Q  │  S  │  D  │     │     │ ⚫⚪⚪⚪⚫⚫
├─────┼─────┼─────┼─────┼─────┼─────┤
│     │     │     │     │     │     │ ⚫⚫⚫⚫⚫⚫
├─────┼─────┼─────┼─────┼─────┼─────┤
│ Alt │  R  │Space│ Br- │ Br+ │     │ 🔴🟣⚪⚪⚪⚫
└─────┴─────┴─────┴─────┴─────┴─────┘
```

**Couleurs RGB :**
- 🔵 **BLEU** : Chiffres &, é
- 🟣 **VIOLET** : Chiffres ", ', ( + touches R
- 🟡 **JAUNE** : Modificateurs (Ctrl, Shift, E)
- ⚪ **BLANC** : ZQSD + Espace + Brightness
- 🔴 **ROUGE** : Alt
- ⚫ **ÉTEINT** : Touches inutilisées

**Contrôles de luminosité intégrés :**
- **Br-** : Diminuer la luminosité RGB
- **Br+** : Augmenter la luminosité RGB

---

## 🚀 Quick Start

### 1️⃣ Compiler le firmware

```bash
./build.sh
```

Ce script :
- Synchronise les fichiers du projet vers QMK
- Compile le firmware
- Copie le `.uf2` dans le projet

### 2️⃣ Flasher le Pico

**Entrer en mode bootloader :**

- 🔹 **Méthode 1 :** Maintenir `BOOT` + appuyer `RESET` + relâcher `BOOT`
- 🔹 **Méthode 2 :** Double-tap rapide sur `RESET`

**Flasher automatiquement :**

```bash
./flash.sh
```

**Ou manuellement :**

```bash
sudo cp firmware.uf2 /media/$USER/RPI-RP2*/
```

✅ Le clavier redémarre automatiquement !

### 3️⃣ Reset EEPROM (première utilisation)

Pour appliquer la configuration par défaut du firmware (luminosité 10%, etc.) :

**Bootmagic Reset :**
1. Débranchez le clavier
2. Maintenez la touche en haut à gauche (position 0,0 - la touche "-")
3. Rebranchez le clavier tout en maintenant la touche
4. L'EEPROM sera réinitialisée

---

## 📁 Structure des fichiers

```
colagan-pico-split-keyboard/
├── 🔨 build.sh                    # Script de compilation + synchronisation
├── ⚡ flash.sh                    # Script de flash automatique
├── 📦 firmware.uf2                # Firmware compilé
├── ⚙️ keyboard.json               # Configuration matérielle QMK
├── 📖 readme.md                   # Ce fichier
└── 🗂️ keymaps/
    └── via/                       # Keymap VIA
        ├── keymap.c               # Layout des touches + RGB custom
        └── config.h               # Configuration (3 layers, luminosité)
```

---

## 🔧 Configuration matérielle

### 📌 Pins GPIO

- **Colonnes (6) :** `GP20`, `GP22`, `GP12`, `GP11`, `GP10`, `GP9`
- **Lignes (5) :** `GP21`, `GP19`, `GP18`, `GP17`, `GP16`
- **RGB LEDs :** `GP13` (SK6812 MINI-E data)

### 💡 RGB Matrix

- **LEDs :** 30 SK6812 MINI-E (1 par touche)
- **Câblage :** Serpentin (ligne 0: L→R, ligne 1: R→L, etc.)
- **Luminosité par défaut :** 10% (26/255)
- **Luminosité max :** 200/255

### 🔌 USB VID/PID

- **Vendor ID :** `0xFEED`
- **Product ID :** `0x0004`
- **Nom :** Colagan V2 Right Only

---

## 🎨 RGB Matrix

### Modes RGB

Le clavier dispose de 2 modes RGB distincts :

1. **Layers BASE/FN** : Animations RGB Matrix standards (40+ effets)
2. **Layer GAMING** : Couleurs fixes personnalisées par touche (voir schéma ci-dessus)

### Contrôles RGB via VIA

Dans VIA (onglet **Lighting** → **QMK RGB Matrix**) :

| Keycode   | Action                         |
|-----------|--------------------------------|
| `RM_TOGG` | 💡 On/Off des LEDs            |
| `RM_NEXT` | ➡️ Animation suivante         |
| `RM_PREV` | ⬅️ Animation précédente       |
| `RM_VALU` | ☀️ Augmenter la luminosité    |
| `RM_VALD` | 🌙 Diminuer la luminosité     |
| `RM_HUEU` | 🔴 Augmenter la teinte        |
| `RM_HUED` | 🔵 Diminuer la teinte         |
| `RM_SATU` | ⬆️ Augmenter la saturation    |
| `RM_SATD` | ⬇️ Diminuer la saturation     |
| `RM_SPDU` | ⚡ Vitesse d'animation +       |
| `RM_SPDD` | 🐌 Vitesse d'animation -       |

**Note :** En mode GAMING, seuls les contrôles de luminosité (Br-/Br+) fonctionnent. Les autres effets sont désactivés pour maintenir les couleurs personnalisées.

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
| `ù`                       | `'` (quote)               |
| `ç`                       | `9`                       |
| `-` (tiret)               | `6`                       |
| `A`                       | `Q`                       |
| `Q`                       | `A`                       |
| `W`                       | `Z`                       |
| `Z`                       | `W`                       |
| `M`                       | `;`                       |
| `,` (virgule)             | `M`                       |

---

## 🛠️ Dépannage

### ❌ VIA ne détecte pas le clavier

1. ✅ Vérifiez que le firmware a été compilé avec `-km via`
2. 🔄 Rechargez la page VIA
3. 🔌 Vérifiez la connexion USB :
   ```bash
   lsusb | grep -i feed
   # Devrait afficher : ID feed:0004 Colagan V2 Right Only
   ```

### 🔤 Les touches ne correspondent pas

➡️ **Normal !** VIA affiche en QWERTY, votre OS est en AZERTY.
📋 Référez-vous au tableau de correspondances ci-dessus.

### 💡 Le clavier démarre avec des couleurs RGB étranges

C'est normal ! Le clavier démarre en layer BASE avec les animations RGB par défaut.

Pour utiliser le mode GAMING :
- Triple-tap sur Ctrl (touche en bas à droite)

Pour désactiver le RGB au démarrage :
- Utilisez VIA pour assigner `RM_TOGG` à une touche et éteindre le RGB

### 🎮 Le layer GAMING ne démarre pas avec la bonne luminosité

Si la luminosité est à 0 après un flash :

1. Débranchez le clavier
2. Maintenez la touche en haut à gauche ("-")
3. Rebranchez le clavier (Bootmagic Reset)
4. La luminosité sera réinitialisée à 10%

### 🔄 Reset de la configuration

**Bootmagic Reset (recommandé) :**
- Débranchez le clavier
- Maintenez la touche en haut à gauche (L0C0)
- Rebranchez le clavier

**Via VIA :**
- Settings ⚙️ → "Clear EEPROM"
- Déconnectez/reconnectez le clavier

---

## 📚 Commandes utiles

```bash
# 🔨 Compiler et synchroniser
./build.sh

# ⚡ Flasher
./flash.sh

# 🧹 Nettoyer les fichiers de compilation
cd ~/qmk_firmware
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

---

## 🎯 Notes de développement

### Custom RGB Gaming Layer

Le layer GAMING utilise `rgb_matrix_indicators_user()` pour définir des couleurs personnalisées par touche :
- Les couleurs sont proportionnelles à la luminosité globale (`rgb_matrix_get_val()`)
- Les contrôles Br+/Br- appellent directement `rgb_matrix_increase_val()` et `rgb_matrix_decrease_val()`
- Le mapping LED suit un pattern serpentin (ligne 0: 0-5, ligne 1: 11-6, ligne 2: 12-17, etc.)

### Multi-tap Ctrl Key

La touche Ctrl (position 4,4) utilise un système de multi-tap :
- Délai de détection : `TAPPING_TERM` (200ms par défaut)
- 1 tap : Ctrl normal
- 2 taps : Toggle layer FN
- 3 taps : Toggle layer GAMING
