

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


#include "Ch10Format_1553.h"



// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

// Construct Transmit, Receive
ClCh10Format_1553::ClCh10Format_1553(unsigned uRT, unsigned bTR, unsigned uSubAddr, unsigned uWC)
    {
    memset(&suIPH, 0, sizeof(Su1553F1_Header));
    suIPH.uGapTime1              = 60;
    suIPH.uGapTime2              =  0;

    suCmdWord1.suStruct.uRTAddr  = uRT;
    suCmdWord1.suStruct.bTR      = bTR;
    suCmdWord1.suStruct.uSubAddr = uSubAddr;

    SetWordCount(uWC);

    memset(&suCmdWord2,  0, sizeof(SuCmdWordU));

    memset(&suStatWord1, 0, sizeof(SuStatWordU));
    suStatWord1.suStruct.uRTAddr = uRT;

    memset(&suStatWord2, 0, sizeof(SuStatWordU));

    }

// ----------------------------------------------------------------------------

// Construct RT to RT message
ClCh10Format_1553::ClCh10Format_1553(unsigned uRT_Send, unsigned uRT_Rcv, unsigned uSubAddr_Send, 
        unsigned uSubAddr_Rcv, unsigned uWC)
    {
    memset(&suIPH, 0, sizeof(Su1553F1_Header));
    suIPH.bRT2RT                 = 1;
    suIPH.uGapTime1              = 60;
    suIPH.uGapTime2              = 60;

    suCmdWord1.suStruct.uRTAddr  = uRT_Rcv;
    suCmdWord1.suStruct.bTR      = 0;
    suCmdWord1.suStruct.uSubAddr = uSubAddr_Rcv;

    suCmdWord2.suStruct.uRTAddr  = uRT_Send;
    suCmdWord2.suStruct.bTR      = 1;
    suCmdWord2.suStruct.uSubAddr = uSubAddr_Send;

    SetWordCount(uWC);

    memset(&suStatWord1, 0, sizeof(SuStatWordU));
    suStatWord1.suStruct.uRTAddr = uRT_Send;

    memset(&suStatWord2, 0, sizeof(SuStatWordU));
    suStatWord2.suStruct.uRTAddr = uRT_Rcv;

    }

// ----------------------------------------------------------------------------

// Construct Mode Code message
ClCh10Format_1553::ClCh10Format_1553(unsigned uRT, unsigned bTR, unsigned uModeCode)
    {
    memset(&suIPH, 0, sizeof(Su1553F1_Header));

    suCmdWord1.suStruct.uRTAddr  = uRT;
    suCmdWord1.suStruct.bTR      = bTR;
    suCmdWord1.suStruct.uSubAddr = 0;

    SetWordCount(uModeCode);

    memset(&suCmdWord2,  0, sizeof(SuCmdWordU));

    memset(&suStatWord1, 0, sizeof(SuStatWordU));
    suStatWord1.suStruct.uRTAddr = uRT;

    memset(&suStatWord2, 0, sizeof(SuStatWordU));

    }


// ----------------------------------------------------------------------------

ClCh10Format_1553::~ClCh10Format_1553()
    {
    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

// Set the relative time counter

void ClCh10Format_1553::SetRTC(int64_t * pullRelTime)
    {
    vLLInt2TimeArray(pullRelTime, suIPH.aubyIntPktTime);
    }


// ----------------------------------------------------------------------------

// Set the word count / mode code for the current message.

void ClCh10Format_1553::SetWordCount(unsigned uWordCnt)
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


