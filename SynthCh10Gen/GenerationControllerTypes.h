#pragma once

#include "Ch10Channel.h"

enum class SourceFileType {
  INVALID = 0,
  CSV,
  SQLITE,
  TSV
};

enum class ControllerStatus {
  OK = 0,
  INVALID_CONFIG,
  OPEN_OUTPUT_FILE_FAILED,
  OPEN_SOURCE_FILE_FAILED,
  INVALID_START_TIME,
  SOURCES_DEPLETED
};

enum class ChannelActionType {
  PUSH = 0,
  COMMIT
};

class ChannelAction {
public:
  ChannelAction(Ch10Channel* channel, ChannelActionType actionType)
    : channel(channel), type(actionType) {};

  Ch10Channel* channel;
  ChannelActionType type;
};

enum class RateUnit {
  TIME_SEC = 0, // seconds
  TIME_MS,      // milliseconds
  TIME_US,      // microseconds
  TIME_RTC,     // 100 nanoseconds | 10 MHz
  TIME_NS,      // nanoseconds
  HERTZ,        // frequency (Hz)
};

class Rate {
public:
  Rate() 
    : value(0), units(RateUnit::TIME_MS) {};
  Rate(int64_t value, RateUnit units)
    : value(value), units(units) {};

  int64_t value;
  RateUnit units;

  void ConvertUnits(RateUnit toUnit) {
    static const uint64_t ONE_BILLION = 1000000000;
    static const uint64_t ONE_MILLION = 1000000;
    static const uint64_t ONE_THOUSAND = 1000;

    if (this->units == toUnit) // nothing to do
      return;

    uint64_t val = this->value;

    // convert to ns
    switch (this->units) {
    case RateUnit::HERTZ:
      val = (uint64_t)((1.0 / (double)val) * ONE_BILLION);
      break;
    case RateUnit::TIME_SEC:
      val *= ONE_BILLION;
      break;
    case RateUnit::TIME_MS:
      val *= ONE_MILLION;
      break;
    case RateUnit::TIME_US:
      val *= ONE_THOUSAND;
      break;
    case RateUnit::TIME_RTC:
      val *= 100;
      break;
    case RateUnit::TIME_NS:
    default:
      break;
    }

    // convert to destination unit
    switch (toUnit) {
    case RateUnit::HERTZ:
      val = (uint64_t)((1.0 / (double)val) * ONE_BILLION);
      break;
    case RateUnit::TIME_SEC:
      val /= ONE_BILLION;
      break;
    case RateUnit::TIME_MS:
      val /= ONE_MILLION;
      break;
    case RateUnit::TIME_US:
      val /= ONE_THOUSAND;
      break;
    case RateUnit::TIME_RTC:
      val /= 100;
      break;
    case RateUnit::TIME_NS:
    default:
      break;
    }

    this->value = val;
    this->units = toUnit;
  }
};

struct ControllerTime {
  double startSimClockTime;    // Starting simulation Date/Time
  double currSimClockTime;     // Current simulation Data/Time
  double nextPrintTime;
};
