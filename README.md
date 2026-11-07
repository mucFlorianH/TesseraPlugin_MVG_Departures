# Another MVG departures for Tessera

Ein Tessera-Plugin, das Abfahrten aus **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Einen Template-Sensor mit den Abfahrtslisten aus dem Beispiel unten anlegen.
4. In Tessera das Plugin hinzufügen, die Kachel platzieren und den Template-Sensor auswählen.
5. Den gewünschten Haltestellen-Namen im Feld **Name auf dem Bildschirm** eintragen.

Tessera überträgt Listen aus Objekten nicht vollständig. Der Template-Sensor stellt die Abfahrtsdaten deshalb als getrennte Listen einfacher Werte bereit. Der Haltestellen-Name steht groß oberhalb der Tabelle; der Tabellenkopf hat einen durchgehenden, hellgrauen Hintergrund. Linien-Badges übernehmen die Farben der Verkehrslinie. Verspätungen erscheinen direkt vor der erwarteten Zeit, durch ein Leerzeichen getrennt und farblich markiert. Die Verkehrsmittel-Symbole neben dem Titel werden standardmäßig aus den anstehenden Abfahrten erkannt. In den Kacheloptionen kannst du stattdessen mehrere Verkehrsmittel auswählen; eine leere Auswahl aktiviert wieder die automatische Erkennung. Kleine Kacheln behalten die wesentlichen Informationen. Bei einspaltigen und einzeiligen Kacheln steht der kleine Haltestellentitel ohne Symbole oben; bei einspaltigen Kacheln entfallen die Tabellenüberschriften. In breiteren, mehrzeiligen Kacheln stehen die Verkehrsmittel-Symbole rechts neben der Überschrift. Schmale Kacheln geben der Linienbezeichnung mehr Platz; in zweispaltigen Kacheln bleibt mehr Breite für das Ziel. In breiteren Kacheln steht das schmale Gleisfeld direkt links neben der Abfahrtszeit. Unbenutzte Zeilen und Trennlinien werden ausgeblendet. Die Kachel zeigt so viele Abfahrten, wie in den verfügbaren Platz passen. Der Haltestellen-Name wird, wenn vorhanden, aus dem optionalen Attribut `station_name` übernommen. Ohne dieses Attribut den Namen im Feld **Name auf dem Bildschirm** jeder Kachel eintragen; als letzte Rückfallebene erscheint „Abfahrten“. Ein Another-MVG-Sensor ohne Template-Helfer liefert weiterhin höchstens zwei Abfahrten.

## Template-Sensor

Ersetze `sensor.another_mvg_departures` durch deine Another-MVG-Entität. Falls bereits ein `template:`-Abschnitt existiert, ergänze den Sensor dort, statt einen zweiten Abschnitt anzulegen.

```yaml
template:
  - triggers:
      - trigger: state
        entity_id: sensor.another_mvg_departures
    variables:
      departures: >
        {{ trigger.to_state.attributes.get('departures', []) if trigger.to_state else [] }}
      mvg_config: >
        {{ trigger.to_state.attributes.get('config', {}) if trigger.to_state else {} }}
    sensor:
      - name: "MVG Abfahrten für Tessera"
        unique_id: mvg_departures_for_tessera
        state: "{{ departures | count }}"
        attributes: >
          {{
            {
              "station_name": mvg_config.get('name', ''),
              "departures": departures | map(attribute='label', default='') | list,
              "departure_type": departures | map(attribute='transport_type', default='') | list,
              "departure_line": departures | map(attribute='label', default='') | list,
              "departure_destination": departures | map(attribute='destination', default='') | list,
              "departure_track": departures | map(attribute='track', default='') | list,
              "departure_expected": departures | map(attribute='expected_departure', default='') | list,
              "departure_delay": departures | map(attribute='delay', default=0) | list,
              "departure_cancelled": departures | map(attribute='cancelled', default=false) | map('string') | list
            }
          }}
```

Nach dem Neuladen der Template-Entitäten den Helfer in Tessera auswählen und den Bildschirm mit Plugin-Version 0.1.26 neu bauen. Für einheitliche Überschriften nach der Ergänzung von `station_name` den Template-Sensor in Home Assistant neu laden. Mehr zur YAML-Konfiguration: [Home Assistant Template-Integration](https://www.home-assistant.io/integrations/template/).
