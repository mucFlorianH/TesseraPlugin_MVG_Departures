#pragma once

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>

namespace esphome::mvg_departures::colors {

struct Badge {
  uint32_t background;
  uint32_t foreground;
};

inline std::string normalized(std::string value) {
  std::string out;
  out.reserve(value.size());
  for (unsigned char ch : value) {
    if (!std::isspace(ch)) out.push_back(static_cast<char>(std::toupper(ch)));
  }
  return out;
}

inline bool contains(const std::string &value, const char *part) {
  return value.find(part) != std::string::npos;
}

// Colours follow the supplied MVV rail map and the line badges used by MVG.
// U7/U8 share the colour of their principal U-Bahn corridor; neither has a
// standalone line colour in the published palette.
inline Badge for_line(const std::string &transport_type, const std::string &line) {
  const std::string type = normalized(transport_type);
  const std::string number = normalized(line);
  constexpr uint32_t WHITE = 0xFFFFFF;

  if (number == "U1" || number == "U7") return {0x52822F, WHITE};
  if (number == "U2" || number == "U8") return {0xC20831, WHITE};
  if (number == "U3") return {0xEC6725, WHITE};
  if (number == "U4") return {0x00A984, WHITE};
  if (number == "U5") return {0xBC7A00, WHITE};
  if (number == "U6") return {0x0065AE, WHITE};

  if (number == "S1") return {0x1FBCE6, WHITE};
  if (number == "S2") return {0x79B833, WHITE};
  if (number == "S3") return {0x962A85, WHITE};
  if (number == "S4") return {0xE41F28, WHITE};
  if (number == "S5") return {0x00527F, WHITE};
  if (number == "S6") return {0x008F5C, WHITE};
  if (number == "S7") return {0x8A372F, WHITE};
  if (number == "S8") return {0x2D2B29, 0xFFCD01};
  if (number == "S20") return {0xF05972, WHITE};

  if (contains(type, "TRAM") || contains(type, "STRASSENBAHN")) {
    if (number == "12") return {0x95348B, WHITE};
    if (number == "14") return {0xE5007D, WHITE};
    if (number == "16") return {0x0065AD, WHITE};
    if (number == "17") return {0x8B563E, WHITE};
    if (number == "18") return {0x0AA537, WHITE};
    if (number == "19") return {0xE3000F, WHITE};
    if (number == "20") return {0x10BAE7, WHITE};
    if (number == "21") return {0xBB7900, WHITE};
    if (number == "23") return {0xBBCF00, WHITE};
    if (number == "25") return {0xF1909C, WHITE};
    if (number == "27" || number == "N27") return {0xF7A600, WHITE};
    if (number == "28") return {WHITE, 0xF7A600};
    if (number == "29") return {WHITE, 0xEC1622};
    if (number == "N17") return {0x8B563E, WHITE};
    if (number == "N19") return {0xE3000F, WHITE};
    if (number == "N20") return {0x10BAE7, WHITE};
  }

  if (number == "N80" || number == "N81") return {WHITE, 0x2F5465};
  if (number == "X30") return {0x798674, WHITE};
  if (number == "X35") return {0xB5A226, WHITE};
  if (number == "X36") return {0x657077, WHITE};
  if (number == "X80") return {0x466DAE, WHITE};

  if (contains(type, "BUS") || contains(type, "EXPRESS")) {
    if (number.size() >= 2 && number[0] == 'N') return {0x325868, WHITE};
    if (number.size() >= 2 && number[0] == 'X') return {0x325868, WHITE};
    if (number.size() <= 2 && number >= "50" && number <= "63") return {0xD3762B, WHITE};
    if (number == "157") return {0x89737E, WHITE};
    if (number == "160") return {0x4D5C52, WHITE};
    if (number == "172") return {0x484E6B, WHITE};
    if (number == "193") return {0xB68790, WHITE};
    if (number == "199") return {0x31567E, WHITE};
    return {0x325868, WHITE};
  }

  // Another MVG may add a new route label before this palette is updated.
  return {0x325868, WHITE};
}

}  // namespace esphome::mvg_departures::colors
