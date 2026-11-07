# Another MVG departures for Tessera

Ein Tessera-Plugin, das Abfahrten aus **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Einen Template-Sensor mit den Abfahrtslisten aus dem Beispiel unten anlegen.
4. In Tessera das Plugin hinzufügen, die Kachel platzieren und den Template-Sensor auswählen.
5. Den gewünschten Titel im Feld **Name auf dem Bildschirm** eintragen.

Tessera überträgt Listen aus Objekten nicht vollständig. Der Template-Sensor stellt die benötigten Abfahrtsdaten deshalb als getrennte Listen einfacher Werte bereit. Die Kachel zeigt bei ausreichend Fläche die Spalten Linie, Ziel, Gleis und Abfahrt. Bei kleineren Kacheln nutzt sie eine kompakte Zeile. Verspätungen erscheinen farblich markiert, die planmäßige Zeit steht darunter. Bei Überlauf zeigt die Kachel die Zahl der weiteren Abfahrten. Ohne Template-Sensor kann ein Another-MVG-Sensor weiterhin bis zu zwei Abfahrten liefern.

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
          departure_planned: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='planned_departure', default='') | list }}
          departure_delay: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='delay', default=0) | list }}
          departure_cancelled: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or [])
               | map(attribute='cancelled', default=false) | map('string') | list }}
```

Die Vorlage erzeugt für jede Abfahrt gleich lange Listen. Nach dem Neuladen der Template-Entitäten den Helfer in Tessera auswählen und den Bildschirm mit der Plugin-Version 0.1.6 neu bauen. Mehr zur YAML-Konfiguration: [Home Assistant Template-Integration](https://www.home-assistant.io/integrations/template/).
