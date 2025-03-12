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

#include "csv_parser.hpp"

#include "Ch10Formatter_PCM.h"

#include <vector>

using namespace Irig106;

// Some helper macros for converting back and forth with semi-circles
#define FLOAT2SEMICIR16(angle)  ((angle) * (double)0x4000     / 90.0)
#define FLOAT2SEMICIR32(angle)  ((angle) * (double)0x40000000 / 90.0)
#define SEMICIR162FLOAT(angle)  ((angle) * 90.0 / 0x4000    )
#define SEMICIR322FLOAT(angle)  ((angle) * 90.0 / 0x40000000)


// ----------------------------------------------------------------------------
// ClCh10Format_PCM_SynthFmtCsv - PCM Synthetic Data Format Generic CSV
// ----------------------------------------------------------------------------

class ClCh10Format_PCM_CSV : public Ch10Formatter_PCM
{

public:
  ClCh10Format_PCM_CSV(float fFrameRate, CSV_FIELDS fields, CSV_FIELDS types);
  ~ClCh10Format_PCM_CSV();

  // Class variables
public:

  // Methods
public:
  void        SetRTC(int64_t* pullRelTime);
  void        FormatMsg(ClSimState* simState);
  std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN, int chanID);

private:
  uint32_t  GetFrameLength(size_t numFields);
  void      InitFrameFieldPointers(CSV_FIELDS fields, CSV_FIELDS types);
  uint32_t  WordSwap(uint32_t value);
  void      ToLower(STR& str);
};

