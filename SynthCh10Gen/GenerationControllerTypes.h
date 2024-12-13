#pragma once

#include "Ch10Channel.h"

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
  FREQUENCY,    // Hz
};

class Rate {
public:
  Rate(int64_t value, RateUnit units)
    : value(value), units(units) {};

  int64_t value;
  RateUnit units;
};

struct ControllerTime {
  double srcTime;              // Nav source data time (seconds)
  double startSimClockTime;    // Starting simulation Date/Time
  double currSimClockTime;     // Current simulation Data/Time
  double nextPrintTime;
};

static void ConvertRateUnits(Rate& rate, RateUnit toUnit) {
  static const uint64_t ONE_BILLION = 1000000000;
  static const uint64_t ONE_MILLION = 1000000;
  static const uint64_t ONE_THOUSAND = 1000;

  if (rate.units == toUnit) // nothing to do
    return;

  uint64_t val = rate.value;

  // convert to ns
  switch (rate.units) {
  case RateUnit::FREQUENCY:
    val = (uint64_t)((1.0 / (double)rate.value) * ONE_BILLION);
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
  case RateUnit::FREQUENCY:
    val = (uint64_t)((1.0 / (double)rate.value) * ONE_BILLION);
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

  rate.value = val;
  rate.units = toUnit;
}
