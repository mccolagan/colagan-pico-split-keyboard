# Protocole « LED stream » du Colagan V2 (raw HID, commande `0xC0`)

Lecture en temps réel de la couleur **réellement affichée** par chacune des 58 LED (après effet RGB Matrix,
couleurs par couche `ledmaps` et luminosité), pour l'aperçu du configurateur web.
Implémentation : `firmware/colagan_v2/led_stream.c`. Outil de test : `tools/led-stream-test.py`.

- **Lecture seule** : aucune écriture EEPROM, aucun changement d'état, aucun déverrouillage Vial requis.
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
| 3 | version du protocole | `1` |
| 4 | `led_count` | `58` |
| 5 | LED max par paquet de couleurs | `9` |
| 6 | positions max par paquet | `13` |
| 7 | `left_count` : LED de la moitié gauche (indices `0…left_count-1`), le reste est à droite | `29` |
| 8 | moitié maître : `0` gauche, `1` droite | `1` au quotidien |
| 9 | drapeaux : bit 0 = split géré par RPC, bit 1 = autre moitié connectée, bit 2 = RGB allumé | `0x07` |
| 10 | luminosité max (`RGB_MATRIX_MAXIMUM_BRIGHTNESS`) | `64` |
| 11 | luminosité actuelle (`val`) | `0…64` |
| 12 | lignes de matrice | `10` |
| 13 | colonnes de matrice | `6` |

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

- Ce sont les octets envoyés aux LED : **luminosité incluse**, donc plafonnés à `max_brightness` (64). Pour
  l'affichage, mettre à l'échelle (ex. `c × 255 / max_brightness`, ou `c × 255 / val` pour retrouver la teinte
  « pleine »). Ne pas afficher les valeurs brutes, l'aperçu serait presque noir.
- RGB éteint, mise en veille ou suspension USB : toutes les LED à `0,0,0` (le drapeau « RGB allumé » de l'info
  distingue l'extinction volontaire).
- Une image peut mêler deux trames d'animation consécutives (l'effet est calculé par tranches entre deux
  requêtes) : sans conséquence visible à 20 i/s. Une LED n'est jamais lue avec la couleur de l'effet à la place
  de sa couleur de couche.

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
