#include "mvg_departures.h"
#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

namespace esphome::mvg_departures {
using tessera::Font;
namespace ui = tessera::ui;

class NextDepartureTile : public tessera::Tile {
 public:
  explicit NextDepartureTile(const tessera::Plugin *) {}

  void create(const tessera::TileContext &c) override {
    title_ = ui::label(c.parent, Font::TITLE, theme::INK);
    lv_obj_set_width(title_, c.width);
    lv_obj_set_pos(title_, 0, 0);

    const int line_height = ui::line_height(Font::BODY);
    const int first_y = ui::line_height(Font::TITLE) + ui::px(6);
    const int max_rows = std::max(1, (c.height - first_y) / line_height);
    rows_.reserve(max_rows);
    for (int i = 0; i < max_rows; ++i) {
      lv_obj_t *row = ui::label(c.parent, Font::BODY, theme::MUTED);
      lv_obj_set_size(row, c.width, line_height);
      lv_obj_set_pos(row, 0, first_y + i * line_height);
      lv_label_set_long_mode(row, LV_LABEL_LONG_CLIP);
      rows_.push_back(row);
    }
  }

  void on_state(JsonObjectConst data) override {
    name_ = data["name"] | "";
    state_ = data["state"] | "";
    wait_ = data["wait"] | "";
    departures_.clear();

    const JsonObjectConst attributes = data["attributes"].as<JsonObjectConst>();
    const JsonArrayConst rows = attributes["departures"].as<JsonArrayConst>();
    for (JsonVariantConst row : rows) {
      if (row.is<const char *>()) {
        const char *text = row.as<const char *>();
        if (text && *text) departures_.emplace_back(text);
      } else if (row.is<JsonObjectConst>()) {
        const JsonObjectConst item = row.as<JsonObjectConst>();
        std::string text = format_departure(item["transport_type"] | "", item["label"] | "",
                                            item["destination"] | "", item["expected_departure"] | "");
        if (!text.empty()) departures_.push_back(text);
      }
    }

    // Tessera currently drops object entries from an entity's list attribute. Keep compatibility with
    // direct Another MVG sensors by using the two scalar records extracted in the manifest.
    if (departures_.empty()) {
      for (int i = 1; i <= 2; ++i) {
        char suffix[4];
        snprintf(suffix, sizeof(suffix), "_%d", i);
        std::string text = format_departure(read(attributes, "transport_type", suffix),
                                            read(attributes, "line", suffix),
                                            read(attributes, "destination", suffix),
                                            read(attributes, "departure_time", suffix));
        if (!text.empty()) departures_.push_back(text);
      }
    }
  }

  void on_tick(uint32_t) override {
    ui::set_text(title_, name_.empty() ? "Abfahrten" : name_);
    if (departures_.empty()) {
      const std::string fallback = !state_.empty() ? state_ :
                                   !wait_.empty() ? "Tessera: " + wait_ : "Keine nächste Abfahrt empfangen";
      ui::set_text(rows_[0], fallback);
      for (size_t i = 1; i < rows_.size(); ++i) ui::set_text(rows_[i], "");
      return;
    }

    const bool overflow = departures_.size() > rows_.size();
    const bool show_overflow = overflow && rows_.size() > 1;
    const size_t shown = show_overflow ? rows_.size() - 1 : std::min(departures_.size(), rows_.size());
    for (size_t i = 0; i < shown; ++i) ui::set_text(rows_[i], departures_[i]);
    if (show_overflow) {
      const size_t hidden = departures_.size() - shown;
      ui::set_text(rows_[shown], "+ " + std::to_string(hidden) + " weitere");
    }
    const size_t clear_from = show_overflow ? shown + 1 : shown;
    for (size_t i = clear_from; i < rows_.size(); ++i) ui::set_text(rows_[i], "");
  }

  void on_theme() override {
    ui::set_color(title_, theme::INK);
    for (lv_obj_t *row : rows_) ui::set_color(row, theme::MUTED);
  }

 private:
  static std::string read(JsonObjectConst attributes, const char *field, const char *suffix) {
    char key[40];
    snprintf(key, sizeof(key), "%s%s", field, suffix);
    return attributes[key] | "";
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

  lv_obj_t *title_{};
  std::vector<lv_obj_t *> rows_;
  std::vector<std::string> departures_;
  std::string name_, state_, wait_;
};

void MvgDepartures::setup() {
  add_tile("probe", [this]() { return new NextDepartureTile(this); });
}

}  // namespace esphome::mvg_departures
