# Another MVG departures for Tessera

Ein Tessera-Plugin, das Abfahrten aus **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Einen Home-Assistant-Template-Sensor mit den aufbereiteten Abfahrtsdaten anlegen (Beispiel unten).
4. In Tessera das Plugin hinzufügen und den Template-Sensor auswählen.

Direkte Another-MVG-Sensoren werden nicht angeboten: Tessera benötigt für diese Kachel einen Template-Sensor mit getrennten Listen einfacher Werte. Das Attribut `departure_line` kennzeichnet einen passenden Sensor. Die Datenfelder werden als parallele Listen übertragen; der Eintrag an derselben Position in jeder Liste gehört zu derselben Abfahrt. `station_name` wird als Haltestellentitel verwendet, wenn das Feld **Name auf dem Bildschirm** leer ist. Ein dort eingetragener Name überschreibt `station_name`; der Eintrag ist optional. Die Feldbeschriftung stammt von Tessera und lässt sich nicht pro Plugin umbenennen.

Die Verkehrsmittel-Symbole werden standardmäßig aus den Abfahrten erkannt. In den Kacheloptionen kannst du stattdessen mehrere Verkehrsmittel auswählen; eine leere Auswahl aktiviert wieder die automatische Erkennung. Kleine Kacheln passen die Darstellung an den verfügbaren Platz an. Verspätungen erscheinen vor der erwarteten Zeit. Abgesagte Fahrten werden als solche angezeigt. Zu lange Ziele laufen langsamer durch. In ein- und zweispaltigen Kacheln wird das Gleis direkt hinter dem Ziel in eckigen Klammern ergänzt (Zahlen mit `Gl.`-Präfix, z. B. `[Gl.2]`, sonst z. B. `[Bus]`). Einspaltige Kacheln zeigen Linie, Abfahrt und danach Ziel/Gleis. Fahrten mit gleicher Linie, erwarteter Zeit, Verspätung, Gleis und Status werden in der Kachel zusammengefasst; unterschiedliche Ziele erscheinen gemeinsam, getrennt durch „ / “. Das Home-Assistant-Template muss dafür nicht geändert werden.

## Template-Sensor

Ersetze `sensor.another_mvg_departures` überall im Beispiel durch die Entity-ID deines Another-MVG-Sensors. Wenn in deiner `configuration.yaml` bereits ein Abschnitt `template:` existiert, füge den Eintrag darunter ein, statt einen zweiten `template:`-Abschnitt anzulegen.

```yaml
template:
  - sensor:
      - name: "Abfahrten für Tessera"
        unique_id: departures_for_tessera
        state: >
          {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | count }}
        attributes:
          station_name: >
            {{ ((state_attr('sensor.another_mvg_departures', 'config') or {}).get('name', '')) }}
          departure_type: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | map(attribute='transport_type', default='') | list }}
          departure_line: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | map(attribute='label', default='') | list }}
          departure_destination: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | map(attribute='destination', default='') | list }}
          departure_track: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | map(attribute='track', default='') | list }}
          departure_expected: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | map(attribute='expected_departure', default='') | list }}
          departure_delay: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | map(attribute='delay', default=0) | list }}
          departure_cancelled: >
            {{ (state_attr('sensor.another_mvg_departures', 'departures') or []) | map(attribute='cancelled', default=false) | map('string') | list }}
```

Nach dem Neuladen der Template-Entitäten sollte der Sensor mit den aufbereiteten Abfahrten in Tessera auswählbar sein. Bei weiteren Sensoren kann derselbe Aufbau mit eigener `name` und eindeutiger `unique_id` wiederholt werden; ändere dabei die Another-MVG-Entity-ID im Beispiel. Mehr zur YAML-Konfiguration: [Home Assistant Template-Integration](https://www.home-assistant.io/integrations/template/).
