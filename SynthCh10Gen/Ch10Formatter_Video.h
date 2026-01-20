#pragma once
#include "Ch10Formatter.h"
#include "FieldSet.h"

class Ch10Formatter_Video :
    public Ch10Formatter
{

public:

  FieldDescriptor videoField;
  std::vector<uint8_t>* videoData;


  virtual void SetRTC(int64_t relTime) = 0;
  virtual void FormatMsg(ClSimState* simState) = 0;
  virtual std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN) = 0;

  int64_t GetRTC()
  {
    return llRelTime;
  }

protected:
  int64_t llRelTime;
};

