# Another MVG departures for Tessera

Ein Tessera-Plugin, das Abfahrten eines ausgewählten Sensors der Home-Assistant-Integration **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Das Plugin zum gewünschten Bildschirm hinzufügen und die Kachel **Departures** platzieren.
4. Einen Sensor mit dem Attribut `departures` auswählen.

## Anzeige

Die Kachel zeigt die nächsten Abfahrten mit Verkehrsmittel, Linie, Ziel und planmäßiger Abfahrtszeit. Tessera überträgt Listen aus Objekten nicht direkt; das Plugin extrahiert die benötigten Felder. Je nach verfügbarer Kachelfläche wird die zweite Abfahrt ebenfalls angezeigt.
