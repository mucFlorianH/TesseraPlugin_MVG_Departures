# Another MVG departures for Tessera

Ein Tessera-Plugin, das Abfahrten aus **Another MVG** als Kachel anzeigt.

## Einrichtung

1. Another MVG in Home Assistant einrichten.
2. Dieses Repository in Tessera über **Plugins → Add with a link** hinzufügen.
3. Für alle Abfahrten einen Template-Sensor anlegen, der das Attribut `departures` als Liste einfacher Textzeilen bereitstellt (Beispiel unten).
4. Das Plugin zur Anzeige hinzufügen und den Template-Sensor als Entität auswählen.

Tessera überträgt Listen aus Objekten nicht vollständig. Der Template-Sensor wandelt sie deshalb in Text um. Die Kachel zeigt so viele Zeilen, wie in ihre aktuelle Fläche passen, und meldet bei Überlauf die Zahl der weiteren Abfahrten. Ohne Template-Sensor kann das Plugin direkt mit einem Another-MVG-Sensor arbeiten und bis zu zwei Abfahrten anzeigen.

## Template-Sensor

Die Quelle in diesem Beispiel ist `sensor.mvg_pappelallee`; ersetze sie durch deine Another-MVG-Entität. Wenn es bereits einen `template:`-Abschnitt in deiner Konfiguration gibt, ergänze den Sensor dort, statt einen zweiten Abschnitt anzulegen.

```yaml
template:
  - sensor:
      - name: "MVG Abfahrten für Tessera"
        unique_id: mvg_departures_for_tessera
        state: >
          {{ (state_attr('sensor.mvg_pappelallee', 'departures') or []) | count }}
        attributes: >
          {% set source = state_attr('sensor.mvg_pappelallee', 'departures') or [] %}
          {% set ns = namespace(rows=[]) %}
          {% for d in source %}
            {% set route = ((d.get('transport_type', '') or '') ~ ' ' ~ (d.get('label', '') or '')) | trim %}
            {% set destination = d.get('destination', '') or '' %}
            {% set departure_time = d.get('expected_departure', '') or '' %}
            {% set ns.rows = ns.rows + [route ~ ' - ' ~ destination ~ ' | ' ~ departure_time] %}
          {% endfor %}
          {{ {"departures": ns.rows} }}
```

Nach dem Neuladen der Template-Entitäten den neuen Sensor in Tessera auswählen. Mehr zur YAML-Konfiguration: [Home Assistant Template-Integration](https://www.home-assistant.io/integrations/template/).
