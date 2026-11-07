#include "mvg_departures.h"
#include <string>

namespace esphome::mvg_departures {
namespace ui = tessera::ui;

class ProbeTile : public tessera::Tile {
 public:
  explicit ProbeTile(const tessera::Plugin *plugin) : plugin_(plugin) {}

  void create(const tessera::TileContext &c) override {
    title_ = ui::label(c.parent, Font::TITLE, theme::INK);
    detail_ = ui::label(c.parent, Font::BODY, theme::MUTED);
    lv_obj_set_width(title_, c.width);
    lv_obj_set_width(detail_, c.width);
    lv_obj_set_pos(title_, 0, 0);
    lv_obj_set_pos(detail_, 0, ui::line_height(Font::TITLE) + ui::px(6));
  }

  void on_state(JsonObjectConst data) override {
    state_ = data["state"] | "";
    name_ = data["name"] | "";
    JsonArrayConst departures = data["attributes"]["departures"].as<JsonArrayConst>();
    count_ = departures.size();
    first_ = "";
    if (!departures.isNull() && count_ > 0) {
      JsonObjectConst first = departures[0].as<JsonObjectConst>();
      const char *line = first["line"] | first["label"] | "?";
      const char *to = first["destination"] | "";
      first_ = std::string(line) + " → " + to;
    }
    wait_ = data["wait"] | "";
  }

  void on_tick(uint32_t) override {
    ui::set_text(title_, name_.empty() ? "Abfahrten" : name_);
    std::string detail;
    if (count_ > 0) {
      detail = std::to_string(count_) + " Abfahrten empfangen";
      if (!first_.empty()) detail += " · " + first_;
    } else if (!wait_.empty()) {
      detail = "Tessera: " + wait_;
    } else {
      detail = "Keine strukturierte departures-Liste empfangen";
    }
    ui::set_text(detail_, detail);
  }

  void on_theme() override {
    ui::set_color(title_, theme::INK);
    ui::set_color(detail_, theme::MUTED);
  }

 private:
  const tessera::Plugin *plugin_;
  lv_obj_t *title_{}, *detail_{};
  std::string state_, name_, first_, wait_;
  size_t count_ = 0;
};

void MvgDepartures::setup() {
  add_tile("probe", [this]() { return new ProbeTile(this); });
}

}  // namespace esphome::mvg_departures
