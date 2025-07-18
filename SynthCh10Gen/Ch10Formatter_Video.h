#pragma once
#include "Ch10Formatter.h"

class Ch10Formatter_Video :
    public Ch10Formatter
{

public:

  std::string fieldName;
  std::vector<uint8_t>* videoData;


  virtual void SetRTC(int64_t* pllRelTime) = 0;
  virtual void FormatMsg(ClSimState* simState) = 0;
  virtual std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN) = 0;

  int64_t* GetRTC()
  {
    return pllRelTime;
  }

protected:
  int64_t* pllRelTime;
};

