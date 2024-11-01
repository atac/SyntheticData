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

enum class RateType {
  TIME = 0,
  FREQUENCY
};

class Rate {
public:
  Rate(int64_t value, RateType type)
    : value(value), type(type) {};

  int64_t value;
  RateType type;
};

struct ControllerTime {
  double srcTime;              // Nav source data time (seconds)
  double startSimClockTime;    // Starting simulation Date/Time
  double currSimClockTime;     // Current simulation Data/Time
  double nextPrintTime;
};

static void ConvertRateUnits(Rate& rate) {
  rate.value = (int)((1.0 / (float)rate.value) * 1000);

  if (rate.type == RateType::FREQUENCY)
    rate.type = RateType::TIME;
  else
    rate.type = RateType::FREQUENCY;
}
