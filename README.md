# Another MVG departures for Tessera

Ein Tessera-Plugin, das die nächste Abfahrt eines ausgewählten Sensors der Home-Assistant-Integration **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Das Plugin zum gewünschten Bildschirm hinzufügen und die Kachel **Departures** platzieren.
4. Einen Sensor mit dem Attribut `departures` auswählen.

## Anzeige

Die Kachel zeigt Verkehrsmittel, Linie, Ziel und planmäßige Abfahrtszeit des ersten Eintrags in `departures`. Tessera überträgt Listen aus Objekten nicht direkt; das Plugin wählt diese Felder deshalb vor der Übergabe einzeln aus. Die Kachel zeigt derzeit nur die nächste Abfahrt.
