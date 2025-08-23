#pragma once

#include <stdlib.h>
#include <cassert>
#include <string>       // std::string

#include "GenerationControllerTypes.h"

#define TIME_COMPARE_MARGIN 0.000002

class ClSimTimer
{
public:
  ClSimTimer(int64_t lTimeoutVal, bool startExpired = false) : lTimer(0), lTimeoutVal(lTimeoutVal), startExpired(startExpired) { }
  ~ClSimTimer() { }

  // Data
public:
  static inline const int64_t  lTicksPerSecond = 10000000; // 10 MHz, same as IRIG RTC
  static inline int64_t  lTicksPerStep = 100000;           // default 10 msec / 100 Hz
  static inline int64_t  lSimClockTicks = 0;
  static inline double   fSimElapsedTime = 0.0;

  std::vector<ChannelAction>* actions = nullptr;

private:
  bool        startExpired;

  int64_t     lTimer;
  int64_t     lTimeoutVal;

  // Methods
public:
  static void Tick() { lSimClockTicks += lTicksPerStep; fSimElapsedTime = (double)lSimClockTicks / (double)lTicksPerSecond; }
  static void Tick(int64_t lStep) { lSimClockTicks += lStep; }

  void InitToSimClock()
  {
    if (startExpired)
      lTimer = lSimClockTicks;
    else
      FromNow();
  }

  bool Expired() { return lSimClockTicks >= lTimer; }
  void FromNow() { this->lTimer = lSimClockTicks + lTimeoutVal; }
  void FromPrev() { this->lTimer += lTimeoutVal; }
  int64_t GetTimeoutValue() { return lTimeoutVal; }
  bool StartsExpired() { return startExpired; }

  void AddAction(ChannelAction action) { // insertion sort actions into vector
    if (actions == nullptr)
      actions = new std::vector<ChannelAction>();

    bool inserted = false;

    for (auto i = actions->begin(); i != actions->end(); i++) {
      if (action.type < i->type) { // sorting comparison
        actions->insert(i, action);
        inserted = true;
        break;
      }
    }

    if (!inserted)
      actions->push_back(action);
  }
};

