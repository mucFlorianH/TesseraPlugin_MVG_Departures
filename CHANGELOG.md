# Changelog

## 0.1.26 - 2026-11-07
- Fix a C++ name collision that prevented the departure component from compiling.


## 0.1.25 - 2026-11-07
- Keep the Tessera manifest within its eight-field limit; read cancellation status through the supported attribute list.


## 0.1.24 - 2026-11-07
- Use a small title without transport markers in one-column and one-row tiles.
- Right-align transport markers beside the title in wider multi-row tiles.
- Tighten the track and departure columns; retain canceled departures without an expected time when cancellation attributes are available.


## 0.1.23 - 2026-11-07
- Put the track before departure time and delay before expected time; right-align cancelled labels.
- Keep the station title and transport markers in one-column tiles while hiding table headings.
- Give one-row tiles a small title above the bottom-aligned departure and improve two-column space for destinations.


## 0.1.22 - 2026-11-07
- Place delay directly after the expected departure time and move the track column to its right.
- Hide station and table headings in one-column layouts; give the route badge more room.


## 0.1.21 - 2026-11-07
- Extend the table header through the tile padding, right-align its departure heading, and tighten the track column.
- Use smaller bus and wider tram markers; disable tap feedback for the display-only tile.
- Remove planned-time data and show delay before the expected time, updating both from sensor changes.


## 0.1.20 - 2026-11-07
- Matched transport markers more closely to the MVG app and centered them beside the station title.
- Extended the table header band and right-aligned departure times.
- Removed the overflow row so every available row can show a departure.


## 0.1.19 - 2026-11-07
- Refined the departure table and responsive one-line layout.
- Added automatic and manually selectable transport mode markers.
- Show expected times with inline, color-coded delay changes.


## 0.1.1 - 2026-11-07
- Made the sensor selection and example departure data generic.
- Documented the root repository layout and entity-data limitation.

## 0.1.0 - 2026-11-07
- Added a technical probe for a sensor entity's departures attribute.
