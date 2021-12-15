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

using namespace Irig106;

class ClCh10Writer_VideoF0
    {
    public:
        ClCh10Writer_VideoF0();
        ~ClCh10Writer_VideoF0();

    // Data
public:
    int                 iHandle;
    unsigned int        uChanID;
    SuI106Ch10Header    suCh10Header;
    SuVideoF0_ChanSpec  suVideoF0CSDW;

    // Methods
    void        Init(int iHandle, unsigned int uChanID);
    std::string TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN, std::string sDescription="");
    void        Write(int64_t * pullRelTime, uint8_t * pBuffer, int iBufferLen);

    };

