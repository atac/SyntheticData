#pragma once

#include <stdlib.h>
#include <cassert>
#include <string>       // std::string

#include "GenerationControllerTypes.h"

class ClSimTimer
{
public:
  ClSimTimer(int64_t lTimeoutVal) : lTimer(0), lTimeoutVal(lTimeoutVal) { }
  ~ClSimTimer() { }

  // Data
public:
  inline static const int64_t  lTicksPerSecond = 10000000; // 10 MHz, same as IRIG RTC
  inline static const int64_t  lTicksPerStep = 100000;     // 10 msec / 100 Hz
  inline static int64_t  lSimClockTicks = 0;
  inline static double   fSimElapsedTime = 0.0;

  int64_t     lTimer;
  int64_t     lTimeoutVal;

  std::vector<ChannelAction>* actions = nullptr;

  // Methods
public:
  static void Tick() { lSimClockTicks += lTicksPerStep; fSimElapsedTime = (double)lSimClockTicks / (double)lTicksPerSecond; }
  static void Tick(int64_t lStep) { lSimClockTicks += lStep; }

  bool Expired() { return lSimClockTicks >= lTimer; }
  void FromNow() { this->lTimer = lSimClockTicks + lTimeoutVal; }
  void FromPrev() { this->lTimer += lTimeoutVal; }
  int64_t GetTimeoutValue() { return lTimeoutVal; }

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

