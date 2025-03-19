

//#include <cstdio>
//#include <cassert>
//#include <ctime>
//#include <string>       // std::string
//#include <iostream>     // std::cout
//#include <sstream>      // std::stringstream
//#include <string.h>
#include <stdlib.h>
#include <memory>
//#include <math.h>


// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_1553f1.h"

using namespace Irig106;


#include "Ch10Formatter_1553.h"



// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

// Construct Transmit, Receive
Ch10Formatter_1553::Ch10Formatter_1553(unsigned uRT, unsigned bTR, unsigned uSubAddr, unsigned uWC)
{
  InitStructs();

  suIPH.uGapTime1 = 60;
  suIPH.uGapTime2 = 0;

  suCmdWord1.suStruct.uRTAddr = uRT;
  suCmdWord1.suStruct.bTR = bTR;
  suCmdWord1.suStruct.uSubAddr = uSubAddr;

  SetWordCount(uWC);

  suStatWord1.suStruct.uRTAddr = uRT;
}

// ----------------------------------------------------------------------------

// Construct RT to RT message
Ch10Formatter_1553::Ch10Formatter_1553(unsigned uRT_Send, unsigned uRT_Rcv, unsigned uSubAddr_Send,
  unsigned uSubAddr_Rcv, unsigned uWC)
{
  InitStructs();

  suIPH.bRT2RT = 1;
  suIPH.uGapTime1 = 60;
  suIPH.uGapTime2 = 60;

  suCmdWord1.suStruct.uRTAddr = uRT_Rcv;
  suCmdWord1.suStruct.bTR = 0;
  suCmdWord1.suStruct.uSubAddr = uSubAddr_Rcv;

  suCmdWord2.suStruct.uRTAddr = uRT_Send;
  suCmdWord2.suStruct.bTR = 1;
  suCmdWord2.suStruct.uSubAddr = uSubAddr_Send;

  SetWordCount(uWC);

  suStatWord1.suStruct.uRTAddr = uRT_Send;

  suStatWord2.suStruct.uRTAddr = uRT_Rcv;
}

// ----------------------------------------------------------------------------

// Construct Mode Code message
Ch10Formatter_1553::Ch10Formatter_1553(unsigned uRT, unsigned bTR, unsigned uModeCode)
{
  InitStructs();

  suCmdWord1.suStruct.uRTAddr = uRT;
  suCmdWord1.suStruct.bTR = bTR;
  suCmdWord1.suStruct.uSubAddr = 0;

  SetWordCount(uModeCode);

  suStatWord1.suStruct.uRTAddr = uRT;
}


// ----------------------------------------------------------------------------

Ch10Formatter_1553::~Ch10Formatter_1553()
    {
    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

// Set the relative time counter

void Ch10Formatter_1553::SetRTC(int64_t * pullRelTime)
    {
    vLLInt2TimeArray(pullRelTime, suIPH.aubyIntPktTime);
    }


// ----------------------------------------------------------------------------

// Set the word count / mode code for the current message.

void Ch10Formatter_1553::SetWordCount(unsigned uWordCnt)
    {
    if (uWordCnt >= 32)
        uWordCnt = 0;

    // RT->BC, BC->RT
    if (suIPH.bRT2RT == 0)
        {
        suCmdWord1.suStruct.uWordCnt = uWordCnt;
        suIPH.uMsgLen = i1553WordCnt(&suCmdWord1) * 2 + 4;
        }
    // RT -> RT
    else
        {
        suCmdWord1.suStruct.uWordCnt = uWordCnt;
        suCmdWord2.suStruct.uWordCnt = uWordCnt;
        suIPH.uMsgLen = i1553WordCnt(&suCmdWord1) * 2 + 8;
        }
    }

// Zero instance data memory
void Ch10Formatter_1553::InitStructs()
{
  memset(auData, 0, sizeof(auData));
  memset(&suIPH, 0, sizeof(Su1553F1_Header));
  memset(&suCmdWord1, 0, sizeof(SuCmdWordU));
  memset(&suCmdWord2, 0, sizeof(SuCmdWordU));
  memset(&suStatWord1, 0, sizeof(SuStatWordU));
  memset(&suStatWord2, 0, sizeof(SuStatWordU));
}
