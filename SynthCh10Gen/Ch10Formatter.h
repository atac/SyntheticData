#pragma once

#include <string>

#include "Common.h"
#include "SimState.h"

class Ch10Formatter
{
public:
  virtual void SetRTC(int64_t* pullRelTime) = 0;
  virtual void FormatMsg(ClSimState* simState) = 0;
  virtual std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN) = 0;
};
