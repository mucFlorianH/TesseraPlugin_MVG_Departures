# Technische Notiz

Das Manifest lässt Sensoren mit einem `departures`-Attribut auswählen. Die aktuelle Kachel zeigt die Anzahl der Abfahrten und die Felder `line` (alternativ `label`) sowie `destination` des ersten Eintrags.

Tessera garantiert nicht, dass Listen aus Objekten bis zur Kachel übertragen werden. Die tatsächliche Datenübergabe muss deshalb mit einem Sensor von Another MVG geprüft werden. `preview.yaml` enthält nur illustrative Plugin-Bench-Daten.

Die Plugin-Dateien liegen direkt im Repository-Stamm; die Firmware-Komponente liegt unter `components/mvg_departures/`.
