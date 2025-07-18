#pragma once

//#include <cstdio>
//#include <cassert>
//#include <ctime>
#include <string>       // std::string
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
//#include "i106_decode_time.h"
#include "i106_decode_video.h"
//#include "i106_decode_tmats.h"

#include "Common.h"
#include "Ch10Formatter_Video.h"

using namespace Irig106;

class Ch10Writer_Video : public Ch10Writer
{
public:
  Ch10Writer_Video();
  ~Ch10Writer_Video();

  // Data
public:
  SuI106Ch10Header    suCh10Header;
  SuVideoF0_ChanSpec* suVideoF0CSDW;

  uint8_t* dataBuf;
  uint32_t bufLen;
  uint32_t currBufOffset;

private:
  Ch10Formatter_Video* formatter;

public:
  // Methods
  void Init(int iHandle, unsigned int uChanID, Ch10Formatter_Video* formatter);
  void AppendMsg();
  void Commit();


  //void        Write(int64_t* pullRelTime, uint8_t* pBuffer, int iBufferLen);


  std::string TMATS(ClTmatsIndexes& TmatsIndex, std::string sCDLN);

};

