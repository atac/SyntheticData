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
#include "i106_decode_pcmf1.h"

#include "Common.h"
#include "SimState.h"

#include "csv_parser.hpp"

#include "Ch10Formatter.h"

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

class ClCh10Format_PCM_SynthFmtCsv : public Ch10Formatter
{
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

public:
  ClCh10Format_PCM_SynthFmtCsv(float fFrameRate, CSV_FIELDS fields, CSV_FIELDS types);
  ~ClCh10Format_PCM_SynthFmtCsv();

  // Class variables
public:
  unsigned int            uChanID;
  SuPcmF1_IntraPktHeader  suIPH;
  std::vector<uint32_t>   pcmFrame;
  std::vector<PcmField>   pcmFields;
  unsigned                uWordLen;       // PCM common word length in bits
  unsigned                uIPHLen;        // IPH length in bytes
  unsigned                uFrameLen;      // PCM frame length in bytes
  float                   fFrameRate;     // Frame rate in Hz

  // Methods
public:
  void SetRTC(int64_t* pullRelTime);
  virtual void MakeMsg(ClSimState* pclSimState);
  virtual std::string TMATS(ClTmatsIndexes& TmatsIndex, std::string sCDLN);

private:
  uint32_t GetFrameLength(size_t numFields);
  void InitFrameFieldPointers(CSV_FIELDS fields, CSV_FIELDS types);
  uint32_t WordSwap(uint32_t value);
  void ToLower(STR& str);
};

