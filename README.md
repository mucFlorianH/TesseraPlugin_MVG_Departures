# Another MVG departures for Tessera

Ein Tessera-Plugin, das Abfahrtsdaten der Home-Assistant-Integration **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Das Plugin zum gewünschten Bildschirm hinzufügen und die Kachel **Departures** platzieren.
4. Einen Sensor auswählen, der ein Attribut `departures` enthält.

## Aktueller Stand

Die Kachel ist eine Datenprobe: Sie zeigt den Namen des ausgewählten Sensors, die Anzahl der empfangenen Abfahrten und – wenn verfügbar – Linie und Ziel der ersten Abfahrt. Sie rendert noch keine vollständige Abfahrtstafel.

Tessera garantiert nicht, dass Listen aus Objekten übertragen werden. Falls die Kachel keine Abfahrten empfängt, kann die Datenstruktur der Grund sein. Die Beispieldaten in `preview.yaml` sind nur für die Plugin-Bench und keine Live-Daten.
