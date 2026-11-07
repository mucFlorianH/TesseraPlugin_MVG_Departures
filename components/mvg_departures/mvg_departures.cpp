#include "mvg_departures.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

namespace esphome::mvg_departures {
using tessera::Font;
namespace ui = tessera::ui;

struct Departure {
  std::string type, line, destination, track, expected, planned, delay, cancelled, summary;
};

struct DepartureRow {
  lv_obj_t *badge{};
  lv_obj_t *line{};
  lv_obj_t *destination{};
  lv_obj_t *track{};
  lv_obj_t *time{};
  lv_obj_t *delay{};
  lv_obj_t *planned{};
  lv_obj_t *summary{};
  lv_obj_t *overflow{};
  lv_obj_t *separator{};
};

class NextDepartureTile : public tessera::Tile {
 public:
  explicit NextDepartureTile(const tessera::Plugin *) {}

  void create(const tessera::TileContext &c) override {
    custom_name_ = c.name ? c.name : "";
    compact_ = c.width < ui::px(380) || c.height < ui::px(210);

    title_ = ui::label(c.parent, Font::TITLE, theme::INK);
    lv_obj_set_size(title_, c.width, ui::line_height(Font::TITLE));
    lv_obj_set_pos(title_, 0, 0);
    lv_label_set_long_mode(title_, LV_LABEL_LONG_DOT);

    const int body_h = ui::line_height(Font::BODY);
    const int title_h = ui::line_height(Font::TITLE);
    const int gap = ui::px(6);

    if (compact_) {
      rows_y_ = title_h + gap * 2;
      row_h_ = std::max(body_h + gap, ui::px(30));
    } else {
      const int line_col = c.width * 15 / 100;
      const int track_col = c.width * 13 / 100;
      const int time_col = c.width * 24 / 100;
      const int destination_x = line_col + gap;
      const int track_x = c.width - time_col - track_col;
      const int time_x = c.width - time_col;

      header_line_ = make_label(c.parent, "Linie", 0, title_h + gap, line_col, body_h, Font::BODY, theme::SUBTLE);
      header_destination_ = make_label(c.parent, "Ziel", destination_x, title_h + gap,
                                       track_x - destination_x - gap, body_h, Font::BODY, theme::SUBTLE);
      header_track_ = make_label(c.parent, "Gleis", track_x, title_h + gap, track_col - gap, body_h,
                                 Font::BODY, theme::SUBTLE);
      header_time_ = make_label(c.parent, "Abfahrt", time_x, title_h + gap, time_col, body_h,
                                Font::BODY, theme::SUBTLE);
      rows_y_ = title_h + gap + body_h + gap;
      row_h_ = std::max(body_h * 2 + gap, ui::px(42));
      line_col_ = line_col;
      destination_x_ = destination_x;
      destination_w_ = track_x - destination_x - gap;
      track_x_ = track_x;
      track_w_ = track_col - gap;
      time_x_ = time_x;
      time_w_ = time_col;
      delay_x_ = time_x + std::max(ui::px(52), time_col * 62 / 100);
      delay_w_ = std::max(ui::px(28), time_x + time_col - delay_x_);
      planned_y_offset_ = body_h;
    }

    const int remaining = c.height - rows_y_;
    const int count = std::max(1, remaining / std::max(1, row_h_));
    rows_.reserve(count);
    for (int i = 0; i < count; ++i) {
      DepartureRow row;
      const int y = rows_y_ + i * row_h_;
      row.separator = ui::block(c.parent, theme::LINE);
      lv_obj_set_size(row.separator, c.width, std::max(1, ui::px(1)));
      lv_obj_set_pos(row.separator, 0, y + row_h_ - std::max(1, ui::px(1)));

      if (compact_) {
        row.summary = make_label(c.parent, "", 0, y, c.width, row_h_, Font::BODY, theme::INK);
      } else {
        const int body_h2 = ui::line_height(Font::BODY);
        row.badge = ui::block(c.parent, theme::CARD);
        lv_obj_set_size(row.badge, std::max(1, line_col_ - ui::px(5)), body_h2 + ui::px(6));
        lv_obj_set_pos(row.badge, 0, y + std::max(0, (body_h2 + ui::px(6)) / 2));

        row.line = make_label(c.parent, "", ui::px(3), y, line_col_ - ui::px(10),
                              body_h2 + ui::px(6), Font::BODY, theme::SLATE);
        row.destination = make_label(c.parent, "", destination_x_, y, destination_w_, body_h2 * 2,
                                     Font::BODY, theme::INK);
        row.track = make_label(c.parent, "", track_x_, y, track_w_, body_h2 * 2,
                               Font::BODY, theme::INK);
        const int main_time_w = std::max(ui::px(52), delay_x_ - time_x_ - ui::px(2));
        row.time = make_label(c.parent, "", time_x_, y, main_time_w, body_h2, Font::TITLE, theme::INK);
        row.delay = make_label(c.parent, "", delay_x_, y, delay_w_, body_h2, Font::BODY, theme::MUTED);
        row.planned = make_label(c.parent, "", time_x_, y + planned_y_offset_, time_w_, body_h2,
                                 Font::BODY, theme::MUTED);
      }
      row.overflow = make_label(c.parent, "", 0, y, c.width, row_h_, Font::BODY, theme::MUTED);
      rows_.push_back(row);
    }
  }

  void on_state(JsonObjectConst data) override {
    entity_name_ = data["name"] | "";
    state_ = data["state"] | "";
    wait_ = data["wait"] | "";
    departures_.clear();

    const JsonObjectConst attributes = data["attributes"].as<JsonObjectConst>();
    const JsonArrayConst lines = attributes["departure_line"].as<JsonArrayConst>();
    if (!lines.isNull() && lines.size() > 0) {
      const JsonArrayConst types = attributes["departure_type"].as<JsonArrayConst>();
      const JsonArrayConst destinations = attributes["departure_destination"].as<JsonArrayConst>();
      const JsonArrayConst tracks = attributes["departure_track"].as<JsonArrayConst>();
      const JsonArrayConst expected = attributes["departure_expected"].as<JsonArrayConst>();
      const JsonArrayConst planned = attributes["departure_planned"].as<JsonArrayConst>();
      const JsonArrayConst delays = attributes["departure_delay"].as<JsonArrayConst>();
      const JsonArrayConst cancelled = attributes["departure_cancelled"].as<JsonArrayConst>();
      for (size_t i = 0; i < lines.size(); ++i) {
        Departure d;
        d.line = array_text(lines, i);
        d.type = array_text(types, i);
        d.destination = array_text(destinations, i);
        d.track = array_text(tracks, i);
        d.expected = array_text(expected, i);
        d.planned = array_text(planned, i);
        d.delay = array_text(delays, i);
        d.cancelled = array_text(cancelled, i);
        if (!d.line.empty() || !d.destination.empty() || !d.expected.empty()) departures_.push_back(d);
      }
    }

    // Older template sensors may expose only preformatted strings; keep those usable.
    if (departures_.empty()) {
      const JsonArrayConst rows = attributes["departures"].as<JsonArrayConst>();
      for (JsonVariantConst row : rows) {
        if (row.is<const char *>()) {
          const char *text = row.as<const char *>();
          if (text && *text) {
            Departure d;
            d.summary = text;
            departures_.push_back(d);
          }
        } else if (row.is<JsonObjectConst>()) {
          const JsonObjectConst item = row.as<JsonObjectConst>();
          Departure d;
          d.type = item["transport_type"] | "";
          d.line = item["label"] | "";
          d.destination = item["destination"] | "";
          d.track = item["track"] | "";
          d.expected = item["expected_departure"] | "";
          d.planned = item["planned_departure"] | "";
          d.delay = array_text(item["delay"]);
          d.cancelled = array_text(item["cancelled"]);
          departures_.push_back(d);
        }
      }
    }

    // For a direct Another MVG sensor, Tessera maps the first two object rows to scalar fields.
    if (departures_.empty()) {
      for (int i = 1; i <= 2; ++i) {
        char suffix[4];
        snprintf(suffix, sizeof(suffix), "_%d", i);
        Departure d;
        d.type = read(attributes, "transport_type", suffix);
        d.line = read(attributes, "line", suffix);
        d.destination = read(attributes, "destination", suffix);
        d.expected = read(attributes, "departure_time", suffix);
        d.summary = format_departure(d.type, d.line, d.destination, d.expected);
        if (!d.summary.empty()) departures_.push_back(d);
      }
    }
  }

  void on_tick(uint32_t) override {
    ui::set_text(title_, !custom_name_.empty() ? custom_name_ :
                           !entity_name_.empty() ? entity_name_ : "Abfahrten");
    const size_t capacity = rows_.size();
    if (departures_.empty()) {
      ui::set_text(rows_[0].overflow, !state_.empty() && state_ != "0" ? state_ :
                                      !wait_.empty() ? "Tessera: " + wait_ : "Keine Abfahrten empfangen");
      clear_cells(rows_[0]);
      for (size_t i = 1; i < capacity; ++i) clear_row(rows_[i]);
      return;
    }

    const bool show_overflow = departures_.size() > capacity && capacity > 1;
    const size_t shown = show_overflow ? capacity - 1 : std::min(departures_.size(), capacity);
    for (size_t i = 0; i < capacity; ++i) {
      if (show_overflow && i == capacity - 1) {
        clear_cells(rows_[i]);
        ui::set_text(rows_[i].overflow, "+ " + std::to_string(departures_.size() - shown) + " weitere");
      } else if (i < shown) {
        ui::set_text(rows_[i].overflow, "");
        show_row(rows_[i], departures_[i]);
      } else {
        clear_row(rows_[i]);
      }
    }
  }

  void on_theme() override {
    ui::set_color(title_, theme::INK);
    if (header_line_) ui::set_color(header_line_, theme::SUBTLE);
    if (header_destination_) ui::set_color(header_destination_, theme::SUBTLE);
    if (header_track_) ui::set_color(header_track_, theme::SUBTLE);
    if (header_time_) ui::set_color(header_time_, theme::SUBTLE);
    for (size_t i = 0; i < rows_.size(); ++i) {
      DepartureRow &row = rows_[i];
      ui::set_fill(row.separator, tessera::Tone::NORMAL, theme::LINE);
      if (row.line) ui::set_color(row.line, theme::SLATE);
      if (row.destination) ui::set_color(row.destination, theme::INK);
      if (row.track) ui::set_color(row.track, theme::INK);
      if (row.time) ui::set_color(row.time, theme::INK);
      if (row.delay) ui::set_color(row.delay, theme::MUTED);
      if (row.planned) ui::set_color(row.planned, theme::MUTED);
      if (row.summary) ui::set_color(row.summary, theme::INK);
      if (row.overflow) ui::set_color(row.overflow, theme::MUTED);
      if (i < departures_.size() && row.badge) set_badge(row.badge, departures_[i].type);
    }
  }

 private:
  static lv_obj_t *make_label(lv_obj_t *parent, const char *text, int x, int y, int width, int height,
                              Font font, theme::Role color) {
    lv_obj_t *label = ui::label(parent, font, color);
    lv_obj_set_size(label, std::max(1, width), std::max(1, height));
    lv_obj_set_pos(label, x, y);
    lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
    ui::set_text(label, text);
    return label;
  }

  static std::string array_text(JsonArrayConst values, size_t index) {
    if (index >= values.size()) return "";
    return array_text(values[index]);
  }

  static std::string array_text(JsonVariantConst value) {
    if (value.is<const char *>()) return value.as<const char *>() ? value.as<const char *>() : "";
    if (value.is<bool>()) return value.as<bool>() ? "true" : "false";
    if (value.is<int>()) return std::to_string(value.as<int>());
    if (value.is<float>()) return std::to_string(value.as<float>());
    return "";
  }

  static std::string read(JsonObjectConst attributes, const char *field, const char *suffix) {
    char key[40];
    snprintf(key, sizeof(key), "%s%s", field, suffix);
    return attributes[key] | "";
  }

  static bool cancelled(const Departure &d) {
    return d.cancelled == "true" || d.cancelled == "True" || d.cancelled == "1" || d.cancelled == "yes";
  }

  static int delay_minutes(const Departure &d) {
    if (d.delay.empty()) return 0;
    char *end = nullptr;
    const long value = std::strtol(d.delay.c_str(), &end, 10);
    return end == d.delay.c_str() ? 0 : static_cast<int>(value);
  }

  static std::string format_departure(const std::string &type, const std::string &line,
                                      const std::string &destination, const std::string &time) {
    std::string text;
    if (!type.empty()) text = type;
    if (!line.empty()) {
      if (!text.empty()) text += " ";
      text += line;
    }
    if (!destination.empty()) {
      if (!text.empty()) text += " - ";
      text += destination;
    }
    if (!time.empty()) {
      if (!text.empty()) text += " | ";
      text += time;
    }
    return text;
  }

  static std::string route(const Departure &d) {
    if (d.type.empty()) return d.line;
    if (d.line.empty()) return d.type;
    return d.type + " " + d.line;
  }

  static void set_badge(lv_obj_t *badge, const std::string &type) {
    const bool bus = type == "BUS" || type == "Bus" || type == "bus";
    ui::set_fill(badge, bus ? tessera::Tone::GOOD : tessera::Tone::ACCENT, theme::CARD);
  }

  void clear_cells(DepartureRow &row) {
    if (row.line) ui::set_text(row.line, "");
    if (row.destination) ui::set_text(row.destination, "");
    if (row.track) ui::set_text(row.track, "");
    if (row.time) ui::set_text(row.time, "");
    if (row.delay) ui::set_text(row.delay, "");
    if (row.planned) ui::set_text(row.planned, "");
    if (row.summary) ui::set_text(row.summary, "");
    if (row.badge) ui::set_fill(row.badge, tessera::Tone::NORMAL, theme::CARD);
  }

  void clear_row(DepartureRow &row) {
    clear_cells(row);
    ui::set_text(row.overflow, "");
  }

  void show_row(DepartureRow &row, const Departure &d) {
    if (compact_) {
      if (!d.summary.empty()) {
        ui::set_text(row.summary, d.summary);
      } else {
        std::string text = route(d);
        if (!d.destination.empty()) {
          if (!text.empty()) text += " · ";
          text += d.destination;
        }
        if (!d.track.empty()) text += " (" + d.track + ")";
        if (cancelled(d)) {
          text += " · Entfällt";
        } else if (!d.expected.empty()) {
          text += " | " + d.expected;
          const int delay = delay_minutes(d);
          if (delay != 0) {
            text += delay > 0 ? " +" : " ";
            text += std::to_string(delay);
            if (!d.planned.empty()) text += " (" + d.planned + ")";
          }
        }
        ui::set_text(row.summary, text);
      }
      return;
    }

    set_badge(row.badge, d.type);
    ui::set_color(row.line, theme::SLATE);
    ui::set_text(row.line, d.line.empty() ? d.type : d.line);
    ui::set_text(row.destination, d.destination);
    ui::set_text(row.track, d.track);
    if (cancelled(d)) {
      ui::set_text(row.time, "Entfällt");
      ui::set_text(row.delay, "");
      ui::set_text(row.planned, "");
      ui::set_tone(row.time, tessera::Tone::ALERT, theme::INK);
      return;
    }

    ui::set_color(row.time, theme::INK);
    ui::set_text(row.time, d.expected);
    const int delay = delay_minutes(d);
    if (delay != 0) {
      ui::set_text(row.delay, std::string(delay > 0 ? "+" : "") + std::to_string(delay));
      ui::set_tone(row.delay, tessera::Tone::ALERT, theme::MUTED);
      ui::set_text(row.planned, d.planned.empty() ? "" : "(" + d.planned + ")");
    } else {
      ui::set_text(row.delay, "");
      ui::set_color(row.delay, theme::MUTED);
      ui::set_text(row.planned, "");
    }
  }

  std::string custom_name_, entity_name_, state_, wait_;
  std::vector<Departure> departures_;
  std::vector<DepartureRow> rows_;
  lv_obj_t *title_{};
  lv_obj_t *header_line_{};
  lv_obj_t *header_destination_{};
  lv_obj_t *header_track_{};
  lv_obj_t *header_time_{};
  int width_{}, rows_y_{}, row_h_{};
  int line_col_{}, destination_x_{}, destination_w_{}, track_x_{}, track_w_{};
  int time_x_{}, time_w_{}, delay_x_{}, delay_w_{}, planned_y_offset_{};
  bool compact_{};
};

void MvgDepartures::setup() {
  add_tile("probe", [this]() { return new NextDepartureTile(this); });
}

}  // namespace esphome::mvg_departures
