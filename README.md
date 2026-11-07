# Another MVG departures for Tessera

Ein Tessera-Plugin, das Abfahrten aus **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Einen Template-Sensor mit den Abfahrtslisten aus dem Beispiel unten anlegen.
4. In Tessera das Plugin hinzufügen, die Kachel platzieren und den Template-Sensor auswählen.
5. Den gewünschten Haltestellen-Namen im Feld **Name auf dem Bildschirm** eintragen.

Tessera überträgt Listen aus Objekten nicht vollständig. Der Template-Sensor stellt die Abfahrtsdaten deshalb als getrennte Listen einfacher Werte bereit. In größeren Kacheln erscheint der Haltestellen-Name als Überschrift über einer Abfahrtstabelle mit Linie, Ziel, Gleis und Abfahrt. Linien-Badges übernehmen die Farben der Verkehrslinie; Verspätungen werden hervorgehoben und die geplante Zeit darunter angezeigt. Bei kleineren Kacheln wechselt die Darstellung in eine kompakte Liste. Wenn mehr Abfahrten vorliegen als in die Kachel passen, wird die Zahl der übrigen angezeigt. Ein Another-MVG-Sensor ohne Template-Helfer liefert weiterhin höchstens zwei Abfahrten.

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

Nach dem Neuladen der Template-Entitäten den Helfer in Tessera auswählen und den Bildschirm mit Plugin-Version 0.1.7 neu bauen. Mehr zur YAML-Konfiguration: [Home Assistant Template-Integration](https://www.home-assistant.io/integrations/template/).
