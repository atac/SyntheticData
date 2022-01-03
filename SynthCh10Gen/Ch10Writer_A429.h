#pragma once

#include <cstdio>
#include <cassert>
#include <ctime>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include <string.h>
#include <stdlib.h>
#include <math.h>

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_time.h"
#include "i106_decode_arinc429.h"
#include "i106_decode_tmats.h"

#include "Common.h"
#include "Ch10Format_A429.h"

using namespace Irig106;


class ClCh10Writer_A429
    {
    public:
        ClCh10Writer_A429();
        ~ClCh10Writer_A429();

    // Structures
#pragma pack(push, 1)
    // 1553 write buffer info
    struct SuWriteMsgA429
        {
        SuI106Ch10Header        suCh10Header;
        SuArinc429F0_ChanSpec * psuA429CSDW;
        unsigned char         * pchDataBuff;    // Make this char * makes pointer math easier
        uint32_t                uBuffLen;       // Size of the write buffer
        } suWriteMsgA429;
#pragma pack(pop)

    // Data
public:
    int                 iHandle;
    unsigned int        uChanID;
    std::string         sCDLN;                  // Linking Channel Data Link Name for TMATS
    int64_t             lPrevMessageTime;       // RTC of the previous packet in the write buffer
    uint32_t            uPrevGapSum;            // Sum of all the gap times in the previous packet

    // Methods
public:
    void Init(int iHandle, unsigned int uChanID);
    std::string TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN, int iTotalBuses, std::string sDescription="");
    void AppendMsg(ClCh10Format_ARINC429 * pA429Msg);

    void Commit();

    };

