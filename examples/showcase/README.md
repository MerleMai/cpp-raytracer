# Showcase-Szenen

Fünf Testszenen, die jeweils einen Aspekt des Raytracers zeigen, sowie eine
Gesamtszene. Alle Szenen teilen sich dieselbe Kameraeinstellung:

- Beobachter bei `(0, 0, -10)`, Schirm bei `z = -4`, Blickrichtung also **+z**
- Schirm 3.2 x 1.8 Längeneinheiten; `dpi` 240 für die Testszenen (768x432)
  und `dpi` 280 für die Gesamtszene (896x504)
- `+x` zeigt im Bild nach rechts, `+y` nach oben

Damit liegt ein Objekt bei `z ≈ 0 … 5` vor der Kamera; die sichtbare halbe
Breite bei Tiefe `z` beträgt `1.6 * (z + 10) / 6`. Bei `z = 0` sind das ±2.67,
bei `z = 5` ±4.0. Objekte weiter außen werden angeschnitten.

Halbräume: `contains` liefert `normal · (x - position) <= 0`, der Körper liegt
also **entgegen** der Normalen. Ein Boden bei `y = -1` bekommt daher
`normal = [0, 1, 0]`, eine Rückwand bei `z = 9` bekommt `normal = [0, 0, -1]`.
Jede Szene hat zusätzlich eine helle Decke bei `y = 9`; ohne sie laufen
Reflexions- und Brechungsstrahlen nach oben ins Leere und liefern Schwarz.

| Datei | Fokus | Worauf zu achten ist |
| --- | --- | --- |
| `01-reflection.json` | Reflexion | Spiegelkugel (`reflected` 0.9) mit roter, grüner und gelber Kugel ringsum; leicht spiegelnder Boden. Im Spiegelbild müssen Boden-Horizont, Decke und die drei Farbkugeln seitenverkehrt erscheinen. `recursion: 4`. |
| `02-refraction.json` | Brechung | Glaskugel (`index 1.5`, `refracted` ~0.93) vor drei Kugeln rot-grün-blau. Durch die Kugel erscheinen sie **spiegelverkehrt** (rot rechts, blau links) und vergrößert. `recursion: 5`. |
| `03-phong-schatten.json` | Phong + Schatten | Vier identische Kugeln, nur `shininess` 2 / 10 / 50 / 250 — der Glanzpunkt wird von links nach rechts kleiner und härter. Eine Lichtquelle, `recursion: 0`, also reine lokale Beleuchtung und harte Schlagschatten. |
| `04-csg.json` | Kombinationen | Links `intersection` (Kugel ∩ geneigter Halbraum = Halbkugel mit sichtbarer Schnittellipse), Mitte `difference` (Kugel minus kleinere Kugel = Schale, Innenfläche mit umgedrehter Normale), rechts `union` zweier überlappender Kugeln mit verschmolzener Silhouette. |
| `05-transformationen.json` | Transformationen | Dreimal dieselbe Einheitskugel: nur skaliert, skaliert + um z gedreht, skaliert + um y gedreht. Reihenfolge der Verschachtelung ist `translation → rotation → scaling → sphere`, damit die Verschiebung nicht mitskaliert wird. Alle drei berühren exakt den Boden. |
| `06-showcase.json` | alles zusammen | Spiegelkugel, Glaskugel, CSG-Schale, transformiertes Ellipsoid und Union in einem Raum aus Boden, Rückwand und Decke, zwei Lichtquellen, `recursion: 5`. |

Rendern:

```bash
./build/raytracer examples/showcase/01-reflection.json out/01-reflection.png
```

Die beiliegenden PNGs sind die Referenz-Renderings dieser Szenen.
