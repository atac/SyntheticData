#pragma once
#include "Ch10Formatter.h"
class Ch10Format_Time :
    public Ch10Formatter
{
public:
  std::string TMATS(ClTmatsIndexes& TmatsIndex, std::string CLDN);
  void        FormatMsg(ClSimState* simState);

  double      GetCurrentTime();

private:
  void        SetRTC(int64_t* pullRelTime) {};

  double currentTime = -1.0; 
};

