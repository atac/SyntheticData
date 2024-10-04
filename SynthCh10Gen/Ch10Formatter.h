#pragma once

#include <string>

#include "Common.h"
#include "SimState.h"

class Ch10Formatter
{
public:
  unsigned int uChanID;

  virtual void SetRTC(int64_t* pullRelTime) = 0;
  virtual void MakeMsg(ClSimState* pclSimState) = 0;
  virtual std::string TMATS(ClTmatsIndexes& TmatsIndex, std::string sCDLN) = 0;
};

