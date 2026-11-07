#include "mvg_departures.h"
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
    lv_obj_set_width(detail_, c.width);
    lv_obj_set_pos(title_, 0, 0);
    lv_obj_set_pos(detail_, 0, ui::line_height(Font::TITLE) + ui::px(6));
  }

  void on_state(JsonObjectConst data) override {
    name_ = data["name"] | "";
    state_ = data["state"] | "";
    const JsonObjectConst attributes = data["attributes"].as<JsonObjectConst>();
    transport_type_ = attributes["transport_type"] | "";
    line_ = attributes["line"] | "";
    destination_ = attributes["destination"] | "";
    departure_time_ = attributes["departure_time"] | "";
    wait_ = data["wait"] | "";
  }

  void on_tick(uint32_t) override {
    ui::set_text(title_, name_.empty() ? "Nächste Abfahrt" : name_);
    std::string detail;
    if (!transport_type_.empty()) detail = transport_type_;
    if (!line_.empty()) {
      if (!detail.empty()) detail += " ";
      detail += line_;
    }
    if (!destination_.empty()) {
      if (!detail.empty()) detail += " → ";
      detail += destination_;
    }
    if (!departure_time_.empty()) {
      if (!detail.empty()) detail += " · ";
      detail += departure_time_;
    }
    if (detail.empty() && !state_.empty()) {
      detail = state_;
    } else if (detail.empty() && !wait_.empty()) {
      detail = "Tessera: " + wait_;
    } else if (detail.empty()) {
      detail = "Keine nächste Abfahrt empfangen";
    }
    ui::set_text(detail_, detail);
  }

  void on_theme() override {
    ui::set_color(title_, theme::INK);
    ui::set_color(detail_, theme::MUTED);
  }

 private:
  lv_obj_t *title_{}, *detail_{};
  std::string name_, state_, transport_type_, line_, destination_, departure_time_, wait_;
};

void MvgDepartures::setup() {
  add_tile("probe", [this]() { return new NextDepartureTile(this); });
}

}  // namespace esphome::mvg_departures
