#pragma once

#include <cstdint>
#include <optional>
#include <string_view>

enum class MediaHideMode : std::uint8_t {
  Off,
  On,
  WhenStopped,
};

inline std::string_view mediaHideModeConfigValue(MediaHideMode mode) {
  switch (mode) {
  case MediaHideMode::Off:
    return "off";
  case MediaHideMode::On:
    return "on";
  case MediaHideMode::WhenStopped:
    return "when_stopped";
  }
  return "off";
}

inline MediaHideMode mediaHideModeFromConfigValue(std::string_view value) {
  if (value == "on") {
    return MediaHideMode::On;
  }
  if (value == "when_stopped") {
    return MediaHideMode::WhenStopped;
  }
  return MediaHideMode::Off;
}