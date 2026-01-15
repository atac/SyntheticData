

//#include <cstdio>
#include <cassert>
//#include <ctime>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <iomanip>
#include <sstream>      // std::stringstream
//#include <string.h>
#include <stdlib.h>
#include <memory>
//#include <math.h>

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_tmats.h"

using namespace std;
using namespace Irig106;

#include "Common.h"
#include "Ch10Format_PCM_CSV.h"

// ----------------------------------------------------------------------------
// ClCh10Format_PCM_SynthFmt1 - PCM Synthetic Data Format 1
//
// Constructor / Destructor
// ----------------------------------------------------------------------------

// Construct PCM packet
ClCh10Format_PCM_CSV::ClCh10Format_PCM_CSV(float fFrameRate, FieldSet fields)
{
  this->uWordLen = 32;  // bits
  this->uIPHLen = 10;  // bytes
  this->uFrameLen = GetFrameLength(fields.size());
  this->fFrameRate = fFrameRate;

  memset(&suIPH, 0, sizeof(SuPcmF1_IntraPktHeader));

  // Init in intrapacket header
  suIPH.uMajorFrStatus = 3;
  suIPH.uMinorFrStatus = 3;

  // Init the PCM data frame
  pcmFrame = vector<uint32_t>(this->uFrameLen, 0);
  pcmFrame[0] = 0x2840FE6B;    // Sync word swapped

  pFrame = &pcmFrame[0];

  // Init frame field pointers
  InitFrameFieldPointers(fields);
}

// ----------------------------------------------------------------------------

ClCh10Format_PCM_CSV::~ClCh10Format_PCM_CSV()
{
}


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------

// Fill in a frame of synthetic PCM Format CSV data from the current sim state

void ClCh10Format_PCM_CSV::FormatMsg(ClSimState* simState)
{
  for (auto iter = pcmFields.begin(); iter != pcmFields.end(); iter++) {
    float v = (float)simState->fState[iter->descriptor.getName()];

    switch (iter->descriptor.getType()) {
    case FieldType::INTEGER_FIELD:
      *iter->pValue = (int32_t)v;
      break;
    case FieldType::FLOAT_FIELD:
    default:
      uint32_t* w = (uint32_t*)&v;
      *w = WordSwap(*w);
      memcpy(iter->pValue, w, 4);
      break;
    }
  }
}


void ClCh10Format_PCM_CSV::InitFrameFieldPointers(FieldSet fields) {
  for (int i = 0; i < fields.size(); i++) {
    PcmField field = PcmField(&pcmFrame[i], fields[i]);
    pcmFields.push_back(field);
  }
}

void ClCh10Format_PCM_CSV::ToLower(STR& str) {
  for (int i = 0; i < strlen(str.data()); i++)
    str[i] = tolower(str[i]);
}

uint32_t ClCh10Format_PCM_CSV::GetFrameLength(size_t numFields) {
  uint32_t words = numFields - 1 + 1; // num fields - time field + framesync
  return words * 4;
}

uint32_t ClCh10Format_PCM_CSV::WordSwap(uint32_t value) {
  uint16_t* w = (uint16_t*)&value;
  uint16_t tmp = w[0];
  w[0] = w[1];
  w[1] = tmp;
  return value;
}

// ----------------------------------------------------------------------------

std::string ClCh10Format_PCM_CSV::TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN)
    {
    std::stringstream   ssTMATS;
    unsigned long       ulDataRate;
    unsigned            uWordsPerMinorFrame;
    unsigned            uBitsPerMinorFrame;
    int                 iMeasIdx;

    // Calculate some parameters
    uBitsPerMinorFrame  = uFrameLen * 8;
    uWordsPerMinorFrame = ((uBitsPerMinorFrame - 32) / uWordLen) + 1;
    ulDataRate          = unsigned long(fFrameRate * float(uBitsPerMinorFrame));
    
    // PCM attributes specific to Synthetic PCM Data Format 1
    ssTMATS <<
        "P-" << TmatsIndex.iPIndex << "\\DLN:" << sCDLN << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\D1:NRZ-L;\n"
        "P-" << TmatsIndex.iPIndex << "\\D2:" << ulDataRate << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\D3:U;\n"
        "P-" << TmatsIndex.iPIndex << "\\D4:N;\n"
        "P-" << TmatsIndex.iPIndex << "\\D5:N;\n"
        "P-" << TmatsIndex.iPIndex << "\\D6:N;\n"
        "P-" << TmatsIndex.iPIndex << "\\D7:N;\n"
        "P-" << TmatsIndex.iPIndex << "\\D8:N/A;\n"
        "P-" << TmatsIndex.iPIndex << "\\TF:ONE;\n"
        "P-" << TmatsIndex.iPIndex << "\\F1:" << uWordLen << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\F2:M;\n"
        "P-" << TmatsIndex.iPIndex << "\\F3:NO;\n"
        "P-" << TmatsIndex.iPIndex << "\\MF\\N:1;\n"
        "P-" << TmatsIndex.iPIndex << "\\MF1:" << uWordsPerMinorFrame << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\MF2:" << uBitsPerMinorFrame  << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\MF3:FPT;\n"
        "P-" << TmatsIndex.iPIndex << "\\MF4:32;\n"
        "P-" << TmatsIndex.iPIndex << "\\MF5:11111110011010110010100001000000;\n"
        "P-" << TmatsIndex.iPIndex << "\\SYNC1:2;\n"
        "P-" << TmatsIndex.iPIndex << "\\SYNC2:0;\n"
        "P-" << TmatsIndex.iPIndex << "\\SYNC3:2;\n"
        "P-" << TmatsIndex.iPIndex << "\\SYNC4:0;\n"
        "P-" << TmatsIndex.iPIndex << "\\ISF\\N:0;\n";
    TmatsIndex.iPIndex++;

    // PCM Measurement Description (D)
    // -------------------------------

    // This stuff is confusing so I break down the first couple in some detail

    ssTMATS <<
        "D-" << TmatsIndex.iDIndex << "\\DLN:" << sCDLN << ";\n"
        "D-" << TmatsIndex.iDIndex << "\\ML\\N:1;\n";

    ssTMATS <<
    // Measurement List 1 - Synthetic PCM Data Format 1;
        "D-" << TmatsIndex.iDIndex << "\\MLN-1:SynthPcmFormat1;\n"
        //                                   ^--- Measurement List Number 1
        "D-" << TmatsIndex.iDIndex << "\\MN\\N-1:" << pcmFields.size() << ";\n";


    // Set the measurand counter
    iMeasIdx = 1;

    for (auto iter = pcmFields.begin(); iter != pcmFields.end(); iter++) {
      D_MEASURAND_1WORD_GENERIC(iter->descriptor.getName(), iMeasIdx, iMeasIdx, "FW")
    }

    assert(iMeasIdx == pcmFields.size() + 1);

    for (auto iter = pcmFields.begin(); iter != pcmFields.end(); iter++) {
      C_CONVERSION_OFFSET_SCALE_GENERIC(iter->descriptor.getName(), iter->descriptor.getName(), "", "FPT", 0.0, 1.0);
    }

    return ssTMATS.str();
    }


