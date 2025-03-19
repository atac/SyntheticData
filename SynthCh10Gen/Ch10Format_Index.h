#pragma once
#include "Ch10Formatter.h"

// irig106lib
#include "irig106ch10.h"

using namespace Irig106;

class Ch10Format_Index :
  public Ch10Formatter
{
public:
  struct RequiredIndexFields {
    uint8_t refTime[6];
    uint32_t packetLen;
    uint16_t chID;
    uint8_t dataType;
  } formatValues;

  Ch10Format_Index();
  ~Ch10Format_Index();

  void Init(SuI106Ch10Header* psuHeader);
  void FormatMsg(ClSimState* simState);
  std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN) { return ""; };

  RequiredIndexFields GetFormatValues();

  void SetRTC(int64_t* pullRelTime) {};

private:
  SuI106Ch10Header* pCh10Header;

};

