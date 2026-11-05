# Protocole « LED stream » du Colagan V2 (raw HID, commande `0xC0`)

Lecture en temps réel de la couleur **réellement affichée** par chacune des 58 LED (après effet RGB Matrix,
couleurs par couche et luminosité), pour l'aperçu du configurateur web. Depuis la version 2 : lecture et
écriture des **couleurs et de l'effet de chaque couche**, et aperçu d'une couche sur le clavier.
Implémentation : `firmware/colagan_v2/led_stream.c` (commandes) et `led_layers.c` (stockage, rendu, split).
Outil de test : `tools/led-stream-test.py`.

- `0x00`–`0x02` sont en **lecture seule**. `0x04`, `0x06` et `0x08` écrivent en EEPROM (immédiatement, sans
  « enregistrer » à part). Aucune sous-commande ne demande le déverrouillage Vial.
- **Requête / réponse** uniquement : le clavier n'envoie jamais rien de lui-même. C'est l'hôte qui fixe la cadence.
- Compatible Vial / vial.rocks : `0xC0` n'est utilisé ni par VIA 9 (`0x01`–`0x13`), ni par Vial (`0xFE`), ni
  par VialRGB (sous-commandes de `0x07` / `0x08`). `0xFF` reste « commande non gérée ».

## Transport

| | |
|---|---|
| Périphérique | VID:PID `FEED:0004`, interface raw HID page d'usage `0xFF60`, usage `0x61` (la même que VIA/Vial) |
| Rapports | 32 octets en entrée et en sortie, sans numéro de rapport (WebHID : `sendReport(0, data)`) |
| Multi-octets | aucun dans la version 1 (tous les champs tiennent sur un octet) |
| Concurrence | un seul canal partagé avec VIA/Vial : **sérialiser** toutes les requêtes dans la même file que le reste du protocole (une requête en vol à la fois) |

## Format général

Requête (32 octets, octets non cités à `0x00`) :

| Octet | Valeur |
|---|---|
| 0 | `0xC0` |
| 1 | sous-commande (`0x00` info, `0x01` positions, `0x02` couleurs) |
| 2 | **`0xFF` obligatoire** (marqueur, voir « Échos ») |
| 3 | `start` : indice de la première LED (ignoré pour info) |

Réponse :

| Octet | Valeur |
|---|---|
| 0 | `0xC0` |
| 1 | sous-commande reprise |
| 2 | statut (voir tableau) |
| 3 | `start` repris (info : début des données) |
| 4 | `count` : nombre d'éléments renvoyés (positions / couleurs) |
| 5… | données |

### Statuts

| Code | Nom | Sens | Données |
|---|---|---|---|
| `0x00` | OK | | valides |
| `0x01` | `UNKNOWN_SUB` | sous-commande inconnue | à zéro |
| `0x02` | `OUT_OF_RANGE` | `start ≥ led_count` | `count = 0` |
| `0x03` | `BAD_REQUEST` | octet 2 ≠ `0xFF` (ou rapport < 32 octets) | à zéro |
| `0x10` | `REMOTE_UNAVAILABLE` | couleurs uniquement : lien split coupé ou RPC en échec | LED de l'autre moitié à `0,0,0`, LED du maître valides |
| `0x11` | `REMOTE_MISMATCH` | couleurs uniquement : l'autre moitié n'a pas ce firmware (pas reflashée) | idem |

Les statuts `0x10` / `0x11` ne sont pas des erreurs fatales : afficher les LED de l'autre moitié comme
« inconnues » (grisées, hachurées…) pour ce paquet.

### Échos et firmware sans la commande

- **Firmware sans `led_stream`** : réponse `[0xFF, sous-commande, 0xFF, start, …]` (octet 0 = `0xFF`,
  comportement standard de VIA). → Fonction indisponible, ne plus interroger.
- **Déverrouillage Vial en cours** (`vial_unlock_start` … `unlock_poll`) : le firmware renvoie la requête
  **telle quelle**. L'octet 2 vaut alors `0xFF`, qui n'est jamais un statut. → Ignorer l'image, réessayer après le
  déverrouillage (ou suspendre l'aperçu pendant le déverrouillage).

## `0x00` : info

Requête : `C0 00 FF 00 00 …`

Données à partir de l'octet 3 :

| Octet | Champ | Valeur actuelle |
|---|---|---|
| 3 | version du protocole | `2` (`1` : firmware sans couleurs par couche, octets 14-16 absents) |
| 4 | `led_count` | `58` |
| 5 | LED max par paquet de couleurs | `9` |
| 6 | positions max par paquet | `13` |
| 7 | `left_count` : LED de la moitié gauche (indices `0…left_count-1`), le reste est à droite | `29` |
| 8 | moitié maître : `0` gauche, `1` droite | `1` au quotidien |
| 9 | drapeaux : bit 0 = split géré par RPC, bit 1 = autre moitié connectée, bit 2 = RGB allumé, bit 3 = couleurs par couche modifiables (v2) | `0x0F` |
| 10 | luminosité max (`RGB_MATRIX_MAXIMUM_BRIGHTNESS`) | `64` |
| 11 | luminosité actuelle (`val`) | `0…64` |
| 12 | lignes de matrice | `10` |
| 13 | colonnes de matrice | `6` |
| 14 | nombre de couches de couleurs (v2) | `8` |
| 15 | couche active pour la frappe, la plus haute (v2) | `0…7` |
| 16 | couche en aperçu sur le clavier, `0xFF` = aucune (v2) | |

Exemple de réponse : `C0 00 00 01 3A 09 0D 1D 01 07 40 28 0A 06 00 …` (val = 40).

Les champs info sont stables pendant une connexion, sauf les drapeaux et `val` : relire l'info de temps en temps
(ex. 1 fois/s) si l'on veut afficher l'état du lien.

## `0x01` : positions (LED → matrice)

Requête : `C0 01 FF <start> 00 …`. Réponse : `count` paires `(ligne, colonne)` à partir de l'octet 5
(`0xFF, 0xFF` = LED sans touche ; aucune sur le Colagan V2). 13 LED max par paquet → 5 requêtes
(`start` = 0, 13, 26, 39, 52) pour les 58 LED. À lire **une fois** à la connexion.

Exemple, `start = 0` :

```
→ C0 01 FF 00 00 …
← C0 01 00 00 0D  00 00  00 01  00 02  00 03  00 04  00 05  01 00  01 01  01 02  01 03  01 04  01 05  02 00  00 …
```

Correspondance actuelle : LED `n` = CH(n+1) ; gauche LED 0-28 = lignes 0-4, droite LED 29-57 = lignes 5-9 ;
`[r, c]` → LED `6r + c` pour les lignes 0-3. Sur la rangée des pouces (lignes 4 et 9), les colonnes vont de 0 à 4
(aucune LED en `[4,5]` ni `[9,5]`). Se fier à la réponse plutôt qu'à cette formule.

## `0x02` : couleurs

Requête : `C0 02 FF <start> 00 …`. Réponse : `count` triplets `(R, G, B)` à partir de l'octet 5, 9 LED max
par paquet. Une image complète = **7 requêtes** (`start` = 0, 9, 18, 27, 36, 45, 54 ; la dernière renvoie
`count = 4`).

Exemple, `start = 27` (effet uni rouge, val 40, droite maître, gauche joignable) :

```
→ C0 02 FF 1B 00 …
← C0 02 00 1B 09  28 00 00  28 00 00  28 00 00  28 00 00  28 00 00  28 00 00  28 00 00  28 00 00  28 00 00
```

Même requête avec le lien split coupé (LED 27-28 de la gauche inconnues, LED 29-35 de la droite valides) :

```
← C0 02 10 1B 09  00 00 00  00 00 00  28 00 00  28 00 00  28 00 00  28 00 00  28 00 00  28 00 00  28 00 00
```

Les LED concernées par `0x10` / `0x11` sont celles du paquet qui n'appartiennent **pas** à la moitié maître
(octets 7 et 8 de l'info).

### Valeurs

- Ce sont les octets envoyés aux LED (PWM), **luminosité incluse**. Ne pas les afficher bruts : l'aperçu serait
  presque noir. Effets RGB Matrix **et** couleurs par couche (`ledmaps`, depuis le firmware du 05/11 qui les
  fait passer par la même courbe) suivent la courbe perceptive CIE 1931 de QMK (`USE_CIE1931_CURVE`) : le
  maximum d'un canal vaut `CIE(val)`, soit **12 pour val = 64**. (Un firmware antérieur sortait les couleurs de
  couche en `palette × val / 255`, jusqu'à 64 : la normalisation ci-dessous les écrête sans changer leur teinte.)
- `CIE(v) = ceil(255 × Y)`, avec `L = v × 100 / 255` et `Y = L / 902,3` si `L ≤ 8`, sinon `((L + 16) / 116)³`.
  Cette formule reproduit exactement les 256 valeurs de la table QMK (`quantum/led_tables.c`). Exemples :
  `CIE(40) = 6`, `CIE(64) = 12`, `CIE(255) = 255`.
- Affichage conseillé : `ref = CIE(val)` (val lu dans l'info), puis pour chaque LED
  `k = min(255 / ref, 255 / max(r, g, b))` et couleur affichée `(r, g, b) × k`. On obtient un rendu à pleine
  luminosité ; une valeur au-dessus de `ref` est écrêtée **en gardant sa teinte** (proportionnellement, pas canal
  par canal). Si `val = 0` ou que toutes les LED sont à 0 : afficher éteint.

```js
const cie = v => { const L = v * 100 / 255; const Y = L <= 8 ? L / 902.3 : ((L + 16) / 116) ** 3; return Math.ceil(255 * Y); };
const display = ([r, g, b], val) => {
  const m = Math.max(r, g, b), ref = cie(val);
  if (!m || !ref) return [0, 0, 0];
  const k = Math.min(255 / ref, 255 / m);
  return [r, g, b].map(c => Math.round(c * k));
};
```
- RGB éteint, mise en veille ou suspension USB : toutes les LED à `0,0,0` (le drapeau « RGB allumé » de l'info
  distingue l'extinction volontaire).
- Une image peut mêler deux trames d'animation consécutives (l'effet est calculé par tranches entre deux
  requêtes) : sans conséquence visible à 20 i/s. Une LED n'est jamais lue avec la couleur de l'effet à la place
  de sa couleur de couche.

## Couleurs et effet par couche (v2)

Chaque couche a, pour chacune des 58 LED, une couleur RGB **pleine luminosité** (0-255) ou le marqueur
« transparent » : la LED montre alors l'effet RGB. Chaque couche peut aussi remplacer l'effet global.
Valeurs par défaut (premier démarrage, bootmagic) : tableau `ledmaps` de `keymaps/vial/keymap.c`.
Le clavier applique la luminosité courante et la courbe CIE au rendu, comme pour les effets.

Format commun des requêtes : `[C0, sous-commande, FF, start, couche, données…]`. Les réponses reprennent
start et couche : `[C0, sous-commande, statut, start, couche, données…]`. Couche ≥ 8 → statut `0x02`.

| Sous-cmd | Rôle | Données de la requête (octet 5…) | Données de la réponse (octet 5…) |
|---|---|---|---|
| `0x03` | lire les couleurs de 8 LED max à partir de `start` | — | `count`, `masque`, `count × (R, G, B)` |
| `0x04` | écrire les couleurs de `count` LED (1 à 8) à partir de `start` | `count`, `masque`, `count × (R, G, B)` | (requête reprise) |
| `0x05` | lire l'effet de la couche | — | `override`, `id` (u16 LE), `vitesse`, `H`, `S`, `V` |
| `0x06` | écrire l'effet de la couche | `override`, `id` (u16 LE), `vitesse`, `H`, `S`, `V` | (requête reprise) |
| `0x07` | aperçu : le clavier affiche la couche `couche` (`0xFF` = arrêter) | — | — |
| `0x08` | remettre la couche aux valeurs par défaut (`8` = toutes) | — | — |

- **Masque** : bit `i` à 1 = LED `start + i` transparente (ses octets RGB sont ignorés à l'écriture).
- **Effet** : `override = 0` → la couche garde l'effet global (celui réglé par VialRGB `0x07 0x41` et enregistré
  par `0x09`). `override = 1` → identifiant **VialRGB** (0 = éteint), vitesse, teinte, saturation et
  luminosité (plafonnée à 64) appliqués tant que la couche est affichée. L'effet global est restauré en sortie.
  Ne pas enregistrer l'effet global (VialRGB `0x09`) pendant qu'une couche à effet propre est affichée : c'est
  l'effet de la couche qui serait enregistré. Le configurateur met en aperçu la couche 0 pour régler l'effet
  global.
- **Aperçu** : le clavier affiche les couleurs **et l'effet** de la couche demandée, sans changer la couche de
  frappe. Il **expire au bout de 3 s** : le renouveler toutes les ~1 s tant que l'édition est ouverte, et
  envoyer `0xFF` en sortie. Une déconnexion USB l'arrête donc toute seule.
- Une image complète d'une couche = 8 requêtes `0x03` (`start` = 0, 8, …, 56 ; la dernière renvoie 2 LED).

Exemple : LED 29-36 de la couche FPS (2), puis écriture de la LED 29 en blanc et de la LED 30 transparente :

```
→ C0 03 FF 1D 02 …
← C0 03 00 1D 02 08 00  00 00 FF  00 00 FF  00 00 FF  00 00 FF  00 00 FF  FF FF 00  FF 00 00  FF FF 00
→ C0 04 FF 1D 02 02 02  FF FF FF  00 00 00 …
← C0 04 00 1D 02 02 02  FF FF FF  00 00 00 …
```

Split : le maître enregistre et pousse aux couleurs de l'esclave (RPC `RPC_ID_KB_LED_LAYERS`) ; un contrôle
toutes les 500 ms resynchronise l'esclave s'il a redémarré ou manqué une écriture. La moitié branchée en USB
fait foi : brancher la gauche seule utilise **ses propres** couleurs enregistrées.

## Cadence conseillée

- **20 images/s** (une image toutes les 50 ms) = 140 rapports/s. Ne pas lancer l'image suivante tant que la
  précédente n'est pas complète. Ne pas dépasser ~30 i/s.
- Prioriser les requêtes d'édition (keymap, RGB) : suspendre ou espacer l'aperçu pendant une écriture et
  pendant le déverrouillage Vial.
- Arrêter l'aperçu quand l'onglet est masqué (`document.visibilityState`).

## Limites split

- Le maître (moitié branchée en USB, la droite au quotidien) lit ses LED dans son tampon WS2812. Les LED de
  l'autre moitié sont lues **à la demande** par un RPC split (`RPC_ID_KB_LED_STREAM`, 2 octets aller,
  28 retour), soit un RPC par paquet qui contient des LED distantes (4 par image, ~80 RPC/s à 20 i/s), environ
  1 ms chacun sur le lien série.
- Les **deux moitiés** doivent avoir ce firmware. Sinon, `0x11` ou `0x10` sur les LED distantes, sans
  conséquence pour le lien ni la frappe.
- Lien coupé ou instable (faux contacts USB-C) : `0x10`. Le firmware ne réessaie pas (pas de latence ajoutée) ;
  l'image suivante retentera.
- Les deux moitiés calculent leurs effets indépendamment (synchronisées par QMK) : l'image renvoyée est bien ce
  que chaque moitié affiche, décalages éventuels compris.

## Évolutions

Toute modification incompatible incrémente la version du protocole (octet 3 de l'info). Le configurateur doit
refuser une version qu'il ne connaît pas plutôt que d'interpréter les données.
