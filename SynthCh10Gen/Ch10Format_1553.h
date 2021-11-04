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
#include "i106_decode_1553f1.h"

#include "Common.h"
#include "SimState.h"

using namespace Irig106;


class ClCh10Format_1553
    {
public:
    ClCh10Format_1553(unsigned uRT, unsigned bTR, unsigned uSubAddr, unsigned uWC);
    ClCh10Format_1553(unsigned uRT_Send,unsigned uRT_Rcv, unsigned uSubAddr_Send, unsigned uSubAddr_Rcv, unsigned uWC);
    ClCh10Format_1553(unsigned uRT, unsigned bTR, unsigned uModeCode);

    virtual ~ClCh10Format_1553();

    // Class variables
public:
    uint16_t            auData[32];
    Su1553F1_Header     suIPH;
    SuCmdWordU          suCmdWord1;
    SuCmdWordU          suCmdWord2;
    SuStatWordU         suStatWord1;
    SuStatWordU         suStatWord2;

    // Methods
public:
    void SetRTC(int64_t * pullRelTime);
    void SetWordCount(unsigned uWordCnt);

    virtual void MakeMsg(ClSimState * pclSimState) = 0;
    virtual std::string TMATS(ClTmatsIndexes & TmatsIndex) = 0;

    };

