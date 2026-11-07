# Another MVG departures for Tessera

Ein Tessera-Plugin, das Abfahrten aus **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Einen Template-Sensor mit den Abfahrtslisten aus dem Beispiel unten anlegen.
4. In Tessera das Plugin hinzufügen, die Kachel platzieren und den Template-Sensor auswählen.
5. Den gewünschten Haltestellen-Namen im Feld **Name auf dem Bildschirm** eintragen.

Tessera überträgt Listen aus Objekten nicht vollständig. Der Template-Sensor stellt die Abfahrtsdaten deshalb als getrennte Listen einfacher Werte bereit. Der Haltestellen-Name steht groß oberhalb der Tabelle; der Tabellenkopf hat einen durchgehenden, hellgrauen Hintergrund. Linien-Badges übernehmen die Farben der Verkehrslinie. Verspätungen erscheinen direkt vor der erwarteten Zeit, durch ein Leerzeichen getrennt und farblich markiert. Die Verkehrsmittel-Symbole neben dem Titel werden standardmäßig aus den anstehenden Abfahrten erkannt. In den Kacheloptionen kannst du stattdessen mehrere Verkehrsmittel auswählen; eine leere Auswahl aktiviert wieder die automatische Erkennung. Kleine Kacheln behalten die wesentlichen Informationen. Bei einspaltigen Kacheln bleibt der Haltestellentitel mit Verkehrsmittel-Symbolen sichtbar, während die Tabellenüberschriften entfallen. Bei einzeiligen Kacheln steht der kleine Haltestellentitel oben und die einzelne Verbindung unten. Schmale Kacheln geben der Linienbezeichnung mehr Platz; in zweispaltigen Kacheln bleibt mehr Breite für das Ziel. In breiteren Kacheln steht das Gleis links neben der Abfahrtszeit. Unbenutzte Zeilen und Trennlinien werden ausgeblendet. Die Kachel zeigt so viele Abfahrten, wie in den verfügbaren Platz passen. Der Haltestellen-Name wird, wenn vorhanden, aus dem optionalen Attribut `station_name` übernommen. Ohne dieses Attribut den Namen im Feld **Name auf dem Bildschirm** jeder Kachel eintragen; als letzte Rückfallebene erscheint „Abfahrten“. Ein Another-MVG-Sensor ohne Template-Helfer liefert weiterhin höchstens zwei Abfahrten.

## Template-Sensor

Ersetze `sensor.another_mvg_departures` durch deine Another-MVG-Entität. Falls bereits ein `template:`-Abschnitt existiert, ergänze den Sensor dort, statt einen zweiten Abschnitt anzulegen.

```yaml
template:
  - sensor:
      - name: "MVG Abfahrten für Tessera"
        unique_id: mvg_departures_for_tessera
        state: >
          {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | count }}
        attributes:
          station_name: >
            {{ ((state_attr('sensor.another_mvg_departures', 'config') or {}).get('name', '')) }}
          departures: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='label', default='') | list }}
          departure_type: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='transport_type', default='') | list }}
          departure_line: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='label', default='') | list }}
          departure_destination: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='destination', default='') | list }}
          departure_track: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='track', default='') | list }}
          departure_expected: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='expected_departure', default='') | list }}
          departure_delay: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='delay', default=0) | list }}
          departure_cancelled: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='cancelled', default=false) | map('string') | list }}
```

Nach dem Neuladen der Template-Entitäten den Helfer in Tessera auswählen und den Bildschirm mit Plugin-Version 0.1.23 neu bauen. Für einheitliche Überschriften nach der Ergänzung von `station_name` den Template-Sensor in Home Assistant neu laden. Mehr zur YAML-Konfiguration: [Home Assistant Template-Integration](https://www.home-assistant.io/integrations/template/).
