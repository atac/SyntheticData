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
#include "i106_decode_tmats_r.h"
#include "i106_decode_pcmf1.h"

#include "Common.h"
#include "SimState.h"

#include "Ch10Formatter.h"

#include <vector>

using namespace Irig106;


// ----------------------------------------------------------------------------
// Ch10Formatter_PCM - Synthetic Data Formatter PCM Superclass
// ----------------------------------------------------------------------------

class Ch10Formatter_PCM : public Ch10Formatter
{
public:

  typedef unsigned char byte;

  enum class FieldType {
    FLOAT_FIELD = 0,
    INTEGER_FIELD
  };

  struct PcmField {
    uint32_t* pValue;
    FieldType type;
    std::string name;
  };

  // Class variables
  SuPcmF1_IntraPktHeader  suIPH;
  uint32_t *              pFrame;         // Pointer to the start of the PCM frame
  unsigned                uWordLen;       // PCM common word length in bits
  unsigned                uIPHLen;        // IPH length in bytes
  unsigned                uFrameLen;      // PCM frame length in bytes
  float                   fFrameRate;     // Frame rate in Hz

  // Methods
public:
  void        FormatMsg(ClSimState* simState) = 0;
  std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN) = 0;


  void SetRTC(int64_t* pullRelTime) 
  {
    vLLInt2TimeArray(pullRelTime, suIPH.aubyIntPktTime);
  }
};

