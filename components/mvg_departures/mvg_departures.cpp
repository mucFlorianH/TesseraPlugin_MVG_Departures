#include "mvg_departures.h"
#include "mvg_colors.h"
#include <algorithm>
#include <cctype>
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
  int y{};
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
    tile_width_ = c.width;
    narrow_ = c.columns <= 1 || c.width < ui::px(460);
    single_line_ = c.rows <= 1 || c.height < ui::px(90);
    compact_ = narrow_ || c.height < ui::px(210);
    title_font_ = compact_ ? Font::HEADLINE : Font::VALUE;
    const int title_h = ui::line_height(title_font_);

    const int gap = ui::px(6);
    const int body_h = ui::line_height(Font::BODY);
    const int line_col = c.width * (narrow_ ? 22 : 15) / 100;
    const int track_col = narrow_ ? 0 : c.width * 13 / 100;
    const int time_col = std::max(ui::px(64), c.width * (narrow_ ? 27 : 24) / 100);
    const int time_x = c.width - time_col;
    const int destination_x = line_col + gap;
    const int track_x = time_x - track_col;
    const int destination_w = std::max(0, (narrow_ ? time_x : track_x - gap) - destination_x);

    if (!single_line_) {
      const int title_h = ui::line_height(title_font_);
      title_ = ui::label(c.parent, title_font_, theme::INK);
      lv_obj_set_size(title_, c.width, title_h);
      lv_obj_set_pos(title_, 0, 0);
      lv_label_set_long_mode(title_, LV_LABEL_LONG_DOT);

      const int header_h = ui::line_height(Font::BODY_LARGE);
      const int header_y = title_h + gap;
      header_bg_ = ui::block(c.parent, theme::TRACK);
      lv_obj_set_size(header_bg_, c.width + ui::px(16), header_h + ui::px(8));
      lv_obj_set_pos(header_bg_, -ui::px(8), header_y - ui::px(4));
      lv_obj_set_style_radius(header_bg_, 0, 0);
      const int header_text_y = header_y + ui::px(2);
      header_line_ = make_label(c.parent, "Linie", 0, header_text_y, line_col, header_h,
                                Font::BODY_LARGE, theme::INK);
      if (destination_w >= ui::px(42))
        header_destination_ = make_label(c.parent, "Ziel", destination_x, header_text_y,
                                          destination_w, header_h, Font::BODY_LARGE, theme::INK);
      if (!narrow_)
        header_track_ = make_label(c.parent, "Gleis", track_x, header_text_y, track_col - gap, header_h,
                                   Font::BODY_LARGE, theme::INK);
      header_time_ = make_label(c.parent, "Abfahrt", time_x, header_text_y, time_col, header_h,
                                Font::BODY_LARGE, theme::INK);
      rows_y_ = header_y + header_h + gap;
    } else {
      rows_y_ = 0;
    }

    const int time_font_h = ui::line_height(compact_ ? Font::BODY_LARGE : Font::TITLE);
    row_h_ = single_line_ ? c.height :
             compact_ ? std::max(body_h + ui::px(12), ui::px(34)) :
                        std::max(time_font_h + body_h + gap, ui::px(52));
    line_col_ = line_col;
    destination_x_ = destination_x;
    destination_w_ = destination_w;
    track_x_ = track_x;
    track_w_ = std::max(0, track_col - gap);
    time_x_ = time_x;
    time_w_ = time_col;
    delay_x_ = time_x + time_col * 66 / 100;
    delay_w_ = std::max(ui::px(20), time_x + time_col - delay_x_);

    int count = single_line_ ? 1 : std::max(1, (c.height - rows_y_) / std::max(1, row_h_));
    rows_.reserve(count);
    for (int i = 0; i < count; ++i) {
      DepartureRow row;
      row.y = rows_y_ + i * row_h_;
      row.separator = ui::block(c.parent, theme::LINE);
      lv_obj_set_size(row.separator, c.width, std::max(1, ui::px(1)));
      lv_obj_set_pos(row.separator, 0, row.y + row_h_ - std::max(1, ui::px(1)));
      lv_obj_add_flag(row.separator, LV_OBJ_FLAG_HIDDEN);
      const int row_body_h = ui::line_height(Font::BODY);
      const int badge_h = row_body_h + ui::px(6);
      const int body_y = row.y + (row_h_ - row_body_h) / 2;
      const int badge_y = row.y + (row_h_ - badge_h) / 2;
      row.badge = ui::block(c.parent, theme::CARD);
      lv_obj_set_size(row.badge, std::max(1, line_col_ - ui::px(5)), badge_h);
      lv_obj_set_pos(row.badge, 0, badge_y);
      row.line = make_label(c.parent, "", 0, body_y, std::max(1, line_col_ - ui::px(5)),
                            row_body_h, Font::BODY, theme::ON_ACCENT);
      lv_obj_set_style_text_align(row.line, LV_TEXT_ALIGN_CENTER, 0);
      row.destination = make_label(c.parent, "", destination_x_, body_y, destination_w_,
                                   row_body_h, Font::BODY, theme::INK);
      if (!narrow_ && !single_line_)
        row.track = make_label(c.parent, "", track_x_, body_y, track_w_, row_body_h,
                               Font::BODY, theme::INK);

      const Font time_font = compact_ ? Font::BODY_LARGE : Font::TITLE;
      const int actual_time_h = ui::line_height(time_font);
      const int time_y = row.y + (row_h_ - actual_time_h) / 2;
      const int main_time_w = std::max(ui::px(40), delay_x_ - time_x_ - ui::px(2));
      row.time = make_label(c.parent, "", time_x_, time_y, main_time_w, actual_time_h,
                            time_font, theme::INK);
      lv_obj_set_style_text_align(row.time, LV_TEXT_ALIGN_RIGHT, 0);
      row.delay = make_label(c.parent, "", delay_x_, time_y + (actual_time_h - row_body_h) / 2,
                             delay_w_, row_body_h, Font::BODY, theme::MUTED);
      row.summary = make_label(c.parent, "", 0, row.y, c.width, row_h_, Font::BODY, theme::INK);
      lv_obj_add_flag(row.summary, LV_OBJ_FLAG_HIDDEN);
      row.overflow = make_label(c.parent, "", 0, row.y, c.width, row_h_, Font::BODY_LARGE, theme::MUTED);
      rows_.push_back(row);
    }

    // Small transport markers beside the title. Empty selection means automatic detection.
    const int icon_widths[] = {40, 32, 32, 52};
    for (int i = 0; i < 4; ++i) {
      const Font icon_font = (i == 1 || i == 2) ? Font::HEADLINE : Font::BODY_LARGE;
      mode_icons_[i] = ui::label(c.parent, icon_font, theme::ACCENT);
      const int icon_h = std::min(ui::px(32), title_h);
      lv_obj_set_size(mode_icons_[i], ui::px(icon_widths[i]), icon_h);
      lv_obj_set_style_text_align(mode_icons_[i], LV_TEXT_ALIGN_CENTER, 0);
      lv_obj_set_style_pad_top(mode_icons_[i],
                               std::max(0, (icon_h - ui::line_height(icon_font)) / 2), 0);
      lv_obj_set_style_border_width(mode_icons_[i], ui::px(1), 0);
      lv_obj_set_style_border_color(mode_icons_[i], lv_color_hex(0xFFFFFF), 0);
      lv_obj_add_flag(mode_icons_[i], LV_OBJ_FLAG_HIDDEN);
    }
    JsonArrayConst selected = c.options["transport_modes"].as<JsonArrayConst>();
    for (JsonVariantConst value : selected) {
      std::string mode = value["value"] | "";
      if (mode.empty()) mode = array_text(value);
      if (mode == "bus" || mode == "u_bahn" || mode == "s_bahn" || mode == "tram")
        manual_modes_.push_back(mode);
    }
  }
  void on_state(JsonObjectConst data) override {
    entity_name_ = data["name"] | "";
    state_ = data["state"] | "";
    wait_ = data["wait"] | "";
    departures_.clear();

    const JsonObjectConst attributes = data["attributes"].as<JsonObjectConst>();
    station_name_ = attributes["station_name"] | "";
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
    const bool custom_title = !custom_name_.empty() && custom_name_ != entity_name_;
    if (!single_line_ && title_) {
      ui::set_text(title_, !station_name_.empty() ? station_name_ :
                             custom_title ? custom_name_ : "Abfahrten");
      update_mode_icons();
    }
    const size_t capacity = rows_.size();
    if (departures_.empty()) {
      clear_cells(rows_[0]);
      lv_obj_clear_flag(rows_[0].overflow, LV_OBJ_FLAG_HIDDEN);
      ui::set_text(rows_[0].overflow, !state_.empty() && state_ != "0" ? state_ :
                                      !wait_.empty() ? "Tessera: " + wait_ : "Keine Abfahrten empfangen");
      for (size_t i = 0; i < capacity; ++i) {
        lv_obj_add_flag(rows_[i].separator, LV_OBJ_FLAG_HIDDEN);
        if (i > 0) clear_row(rows_[i]);
      }
      return;
    }

    const size_t shown = std::min(departures_.size(), capacity);
    const size_t content_rows = shown;
    for (size_t i = 0; i < capacity; ++i) {
      DepartureRow &row = rows_[i];
      if (i < shown) {
        lv_obj_add_flag(row.overflow, LV_OBJ_FLAG_HIDDEN);
        show_row(row, departures_[i]);
      } else {
        clear_row(row);
      }
      if (i + 1 < content_rows) lv_obj_clear_flag(row.separator, LV_OBJ_FLAG_HIDDEN);
      else lv_obj_add_flag(row.separator, LV_OBJ_FLAG_HIDDEN);
    }
  }
  void on_theme() override {
    if (title_) ui::set_color(title_, theme::INK);
    if (header_bg_) ui::set_fill(header_bg_, tessera::Tone::NORMAL, theme::TRACK);
    if (header_line_) ui::set_color(header_line_, theme::INK);
    if (header_destination_) ui::set_color(header_destination_, theme::INK);
    if (header_track_) ui::set_color(header_track_, theme::INK);
    if (header_time_) ui::set_color(header_time_, theme::INK);
    update_mode_icons();
    const size_t capacity = rows_.size();
    const size_t visible = std::min(departures_.size(), capacity);
    for (size_t i = 0; i < rows_.size(); ++i) {
      DepartureRow &row = rows_[i];
      ui::set_fill(row.separator, tessera::Tone::NORMAL, theme::LINE);
      if (row.line) ui::set_color(row.line, theme::ON_ACCENT);
      if (row.destination) ui::set_color(row.destination, theme::INK);
      if (row.track) ui::set_color(row.track, theme::INK);
      if (row.time) ui::set_color(row.time, theme::INK);
      if (row.delay) ui::set_color(row.delay, theme::MUTED);
      if (row.planned) ui::set_color(row.planned, theme::MUTED);
      if (row.summary) ui::set_color(row.summary, theme::INK);
      if (row.overflow) ui::set_color(row.overflow, theme::MUTED);
      if (i < visible && row.badge) set_badge(row, departures_[i]);
    }
  }

 private:
  void update_mode_icons() {
    if (!title_ || single_line_) return;
    std::vector<std::string> modes;
    if (!manual_modes_.empty()) {
      modes = manual_modes_;
    } else {
      for (const auto &d : departures_) {
        std::string type = d.type;
        std::transform(type.begin(), type.end(), type.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        std::string mode;
        if (type.find("bus") != std::string::npos || type.find("express") != std::string::npos) mode = "bus";
        else if (type == "u" || type == "ubahn" || type.find("u-bahn") != std::string::npos ||
                 type.find("subway") != std::string::npos) mode = "u_bahn";
        else if (type == "s" || type == "sbahn" || type.find("s-bahn") != std::string::npos ||
                 type.find("commuter") != std::string::npos) mode = "s_bahn";
        else if (type.find("tram") != std::string::npos || type.find("streetcar") != std::string::npos) mode = "tram";
        if (!mode.empty() && std::find(modes.begin(), modes.end(), mode) == modes.end()) modes.push_back(mode);
      }
    }
    const char *labels[] = {"BUS", "U", "S", "Tram"};
    const char *keys[] = {"bus", "u_bahn", "s_bahn", "tram"};
    const int widths[] = {40, 32, 32, 52};
    const uint32_t fills[] = {0x005B67, 0x126EAF, 0x4D9345, 0xF20D2A};
    int x = 0;
    int shown = 0;
    for (int i = 0; i < 4; ++i) {
      const bool visible = std::find(modes.begin(), modes.end(), keys[i]) != modes.end();
      if (!mode_icons_[i]) continue;
      if (visible) {
        lv_obj_clear_flag(mode_icons_[i], LV_OBJ_FLAG_HIDDEN);
        const int w = ui::px(widths[i]);
        const int h = std::min(ui::px(32), ui::line_height(title_font_));
        lv_obj_set_size(mode_icons_[i], w, h);
        lv_obj_set_pos(mode_icons_[i], x, (ui::line_height(title_font_) - h) / 2);
        lv_obj_set_style_radius(mode_icons_[i], (i == 0 || i == 2) ? h / 2 : ui::px(4), 0);
        lv_obj_set_style_bg_opa(mode_icons_[i], LV_OPA_COVER, 0);
        lv_obj_set_style_bg_color(mode_icons_[i], lv_color_hex(fills[i]), 0);
        lv_obj_set_style_text_color(mode_icons_[i], lv_color_hex(0xFFFFFF), 0);
        ui::set_text(mode_icons_[i], labels[i]);
        x += w + ui::px(4);
        ++shown;
      } else {
        lv_obj_add_flag(mode_icons_[i], LV_OBJ_FLAG_HIDDEN);
      }
    }
    const int offset = shown ? x : 0;
    lv_obj_set_pos(title_, offset, 0);
    lv_obj_set_width(title_, std::max(1, tile_width_ - offset));
  }

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

  static void set_badge(DepartureRow &row, const Departure &d) {
    const colors::Badge palette = colors::for_line(d.type, d.line);
    lv_obj_set_style_bg_color(row.badge, lv_color_hex(palette.background), 0);
    lv_obj_set_style_bg_opa(row.badge, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(row.line, lv_color_hex(palette.foreground), 0);
    lv_obj_set_style_text_align(row.line, LV_TEXT_ALIGN_CENTER, 0);
  }

  void clear_cells(DepartureRow &row) {
    if (row.line) { ui::set_text(row.line, ""); lv_obj_add_flag(row.line, LV_OBJ_FLAG_HIDDEN); }
    if (row.destination) { ui::set_text(row.destination, ""); lv_obj_add_flag(row.destination, LV_OBJ_FLAG_HIDDEN); }
    if (row.track) { ui::set_text(row.track, ""); lv_obj_add_flag(row.track, LV_OBJ_FLAG_HIDDEN); }
    if (row.time) { ui::set_text(row.time, ""); lv_obj_add_flag(row.time, LV_OBJ_FLAG_HIDDEN); }
    if (row.delay) { ui::set_text(row.delay, ""); lv_obj_add_flag(row.delay, LV_OBJ_FLAG_HIDDEN); }
    if (row.planned) { ui::set_text(row.planned, ""); lv_obj_add_flag(row.planned, LV_OBJ_FLAG_HIDDEN); }
    if (row.summary) { ui::set_text(row.summary, ""); lv_obj_add_flag(row.summary, LV_OBJ_FLAG_HIDDEN); }
    if (row.badge) lv_obj_add_flag(row.badge, LV_OBJ_FLAG_HIDDEN);
  }

  void clear_row(DepartureRow &row) {
    clear_cells(row);
    ui::set_text(row.overflow, "");
    lv_obj_add_flag(row.overflow, LV_OBJ_FLAG_HIDDEN);
  }

  void show_row(DepartureRow &row, const Departure &d) {
    if (!d.summary.empty()) {
      clear_cells(row);
      lv_obj_clear_flag(row.summary, LV_OBJ_FLAG_HIDDEN);
      ui::set_text(row.summary, d.summary);
      return;
    }

    lv_obj_add_flag(row.summary, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(row.badge, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(row.line, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(row.time, LV_OBJ_FLAG_HIDDEN);
    set_badge(row, d);
    ui::set_text(row.line, d.line.empty() ? d.type : d.line);
    // The line badge and the expected time stay visible in a one-row tile.
    const bool show_destination = destination_w_ >= ui::px(42);
    if (show_destination && row.destination) {
      lv_obj_clear_flag(row.destination, LV_OBJ_FLAG_HIDDEN);
      ui::set_text(row.destination, d.destination);
    } else if (row.destination) {
      lv_obj_add_flag(row.destination, LV_OBJ_FLAG_HIDDEN);
    }
    if (row.track && !single_line_) {
      lv_obj_clear_flag(row.track, LV_OBJ_FLAG_HIDDEN);
      ui::set_text(row.track, d.track);
    }

    if (cancelled(d)) {
      ui::set_text(row.time, "Entfällt");
      ui::set_text(row.delay, "");
      lv_obj_add_flag(row.delay, LV_OBJ_FLAG_HIDDEN);
      ui::set_tone(row.time, tessera::Tone::ALERT, theme::INK);
      return;
    }

    const int delay = delay_minutes(d);
    ui::set_text(row.time, d.expected);
    if (delay != 0 && delay_w_ >= ui::px(38)) {
      ui::set_text(row.delay, std::string("(") + (delay > 0 ? "+" : "") +
                               std::to_string(delay) + ")");
      lv_obj_clear_flag(row.delay, LV_OBJ_FLAG_HIDDEN);
      ui::set_tone(row.delay, delay > 0 ? tessera::Tone::ALERT : tessera::Tone::GOOD, theme::MUTED);
    } else {
      ui::set_text(row.delay, "");
      lv_obj_add_flag(row.delay, LV_OBJ_FLAG_HIDDEN);
    }
  }
  std::string custom_name_, entity_name_, station_name_, state_, wait_;
  std::vector<std::string> manual_modes_;
  lv_obj_t *mode_icons_[4]{};
  int tile_width_{};
  std::vector<Departure> departures_;
  std::vector<DepartureRow> rows_;
  lv_obj_t *title_{};
  lv_obj_t *header_bg_{};
  lv_obj_t *header_line_{};
  lv_obj_t *header_destination_{};
  lv_obj_t *header_track_{};
  lv_obj_t *header_time_{};
  Font title_font_{Font::HEADLINE};
  int rows_y_{}, row_h_{};
  int line_col_{}, destination_x_{}, destination_w_{}, track_x_{}, track_w_{};
  int time_x_{}, time_w_{}, delay_x_{}, delay_w_{};
  bool compact_{}, narrow_{}, single_line_{};
};

void MvgDepartures::setup() {
  add_tile("probe", [this]() { return new NextDepartureTile(this); });
}

}  // namespace esphome::mvg_departures
