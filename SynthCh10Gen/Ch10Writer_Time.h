#pragma once

//#include <cstdio>
//#include <cassert>
//#include <ctime>
//#include <string>       // std::string
//#include <iostream>     // std::cout
//#include <sstream>      // std::stringstream
//#include <string.h>
//#include <stdlib.h>
//#include <math.h>

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_time.h"
#include "i106_decode_1553f1.h"
//#include "i106_decode_tmats.h"

#include "Ch10Format_Time.h"

using namespace Irig106;

class ClCh10Writer_Time :
  public Ch10Writer
{
public:
  ClCh10Writer_Time();
  ~ClCh10Writer_Time();

  // Structures
#pragma pack(push, 1)
    // Time write buffer info
  struct SuWritePktTimeF1
  {
    SuI106Ch10Header        suCh10Header;
    struct
    {
      SuTimeF1_ChanSpec   suTimeF1CSDW;
      SuTime_MsgDmyFmt    suTimeDataBuff;
      unsigned char       abyFillChksum[8];   // Just for safety
    } suTimePktBuffer;
  } suWritePktTimeF1;
#pragma pack(pop)

  // Data
private:
  Ch10Format_Time* formatter;
  double currentTime;

public:

  // Methods
  void        Init(int iHandle, unsigned int uChanID, Ch10Format_Time* formatter);
  void        AppendMsg() {};
  void        Commit();

  std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN);

  void        SetRelTime(int64_t lSimClockTicks, double fSimClockTime);
};

