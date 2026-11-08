# Changelog

## 0.1.37 - 2026-11-08
- Restore destination text rendering by preserving label dimensions and setting text before enabling marquee mode.

## 0.1.36 - 2026-11-08
- Enable circular marquee mode on destination labels when shown (rendering regression fixed in 0.1.37).
- Give 1x1 tiles a compact layout: line and departure at the top, destination and track scrolling across the bottom, without a station title.

## 0.1.35 - 2026-11-08
- Fix the marquee animation configuration for the LVGL API used by Tessera.
- Add longer pauses before and between circular marquee passes.

## 0.1.34 - 2026-11-08
- Use a per-label marquee animation (the later compatibility fix is in 0.1.35).
- Combine the destination and track in one- and two-column tiles; numeric tracks use the `Gl.` prefix.
- In one-column tiles, show line, departure time, then the scrolling destination and track.

## 0.1.33 - 2026-11-08
- Scroll destinations that do not fit their column.
- Merge matching departures and combine distinct destinations in the tile.


## 0.1.32 - 2026-11-08
- Vertically center the station title and transport markers in the full title area.
- Slightly reduce departure row heights to fit more rows.


## 0.1.31 - 2026-11-08
- Increase the station-title area and center its existing title vertically.


## 0.1.30 - 2026-11-08
- Give the table heading band more height and center its unchanged text vertically.


## 0.1.29 - 2026-11-08
- Use only two station-title sizes: compact for one-column or one-row tiles, and a consistent smaller heading for all other tiles.


## 0.1.28 - 2026-11-08
- Use a custom on-screen name as the station title; fall back to the selected sensor's `station_name` when it is blank.


## 0.1.27 - 2026-11-08
- Offer only flattened Home Assistant template sensors; remove support for selecting raw Another MVG entities.


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
