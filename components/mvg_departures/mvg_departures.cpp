#include "mvg_departures.h"
#include <cstdio>
#include <string>

namespace esphome::mvg_departures {
using tessera::Font;
namespace ui = tessera::ui;

class NextDepartureTile : public tessera::Tile {
 public:
  explicit NextDepartureTile(const tessera::Plugin *) {}

  void create(const tessera::TileContext &c) override {
    title_ = ui::label(c.parent, Font::TITLE, theme::INK);
    detail_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    lv_obj_set_width(title_, c.width);
    lv_obj_set_pos(title_, 0, 0);

    const int title_height = ui::line_height(Font::TITLE);
    const int line_height = ui::line_height(Font::BODY);
    const int first_y = title_height + ui::px(6);
    lv_obj_set_size(detail_, c.width, line_height);
    lv_obj_set_pos(detail_, 0, first_y);
    lv_label_set_long_mode(detail_, LV_LABEL_LONG_CLIP);

    const int second_y = first_y + line_height + ui::px(4);
    if (second_y + line_height <= c.height) {
      later_ = ui::label(c.parent, Font::BODY, theme::MUTED);
      lv_obj_set_size(later_, c.width, line_height);
      lv_obj_set_pos(later_, 0, second_y);
      lv_label_set_long_mode(later_, LV_LABEL_LONG_CLIP);
    }
  }

  void on_state(JsonObjectConst data) override {
    name_ = data["name"] | "";
    state_ = data["state"] | "";
    const JsonObjectConst attributes = data["attributes"].as<JsonObjectConst>();
    read_departure(attributes, "_1", type_1_, line_1_, destination_1_, time_1_);
    read_departure(attributes, "_2", type_2_, line_2_, destination_2_, time_2_);
    wait_ = data["wait"] | "";
  }

  void on_tick(uint32_t) override {
    ui::set_text(title_, name_.empty() ? "Abfahrten" : name_);
    std::string first = format_departure(type_1_, line_1_, destination_1_, time_1_);
    if (first.empty()) first = !state_.empty() ? state_ : !wait_.empty() ? "Tessera: " + wait_ : "Keine nächste Abfahrt empfangen";
    ui::set_text(detail_, first);
    if (later_) ui::set_text(later_, format_departure(type_2_, line_2_, destination_2_, time_2_));
  }

  void on_theme() override {
    ui::set_color(title_, theme::INK);
    ui::set_color(detail_, theme::MUTED);
    if (later_) ui::set_color(later_, theme::MUTED);
  }

 private:
  static void read_departure(JsonObjectConst attributes, const char *suffix, std::string &type, std::string &line,
                             std::string &destination, std::string &time) {
    char key[32];
    snprintf(key, sizeof(key), "transport_type%s", suffix);
    type = attributes[key] | "";
    snprintf(key, sizeof(key), "line%s", suffix);
    line = attributes[key] | "";
    snprintf(key, sizeof(key), "destination%s", suffix);
    destination = attributes[key] | "";
    snprintf(key, sizeof(key), "departure_time%s", suffix);
    time = attributes[key] | "";
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

  lv_obj_t *title_{}, *detail_{}, *later_{};
  std::string name_, state_, wait_;
  std::string type_1_, line_1_, destination_1_, time_1_;
  std::string type_2_, line_2_, destination_2_, time_2_;
};

void MvgDepartures::setup() {
  add_tile("probe", [this]() { return new NextDepartureTile(this); });
}

}  // namespace esphome::mvg_departures
