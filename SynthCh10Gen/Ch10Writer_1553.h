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
#include "i106_decode_1553f1.h"
#include "i106_decode_tmats.h"

#include "Common.h"
#include "Ch10Format_1553.h"

using namespace Irig106;


class ClCh10Writer_1553
    {
    public:
        ClCh10Writer_1553();
        ~ClCh10Writer_1553();

    // Structures
#pragma pack(push, 1)
    // 1553 write buffer info
    struct SuWriteMsg1553
        {
        SuI106Ch10Header        suCh10Header;
        Su1553F1_ChanSpec     * psu1553CSDW;
        unsigned char         * pchDataBuff;    // Make this char * makes pointer math easier
        uint32_t                uBuffLen;       // Size of the write buffer
        } suWriteMsg1553;
#pragma pack(pop)

    // Data
public:
    int                 iHandle;
    unsigned int        uChanID;
    std::string         sCDLN;                  // Linking Channel Data Link Name for TMATS

    // Methods
public:
    void Init(int iHandle, unsigned int uChanID);
    std::string TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN, std::string sDescription="");
    void AppendMsg(Su1553F1_Header * psu1553IPH, int32_t iCmdWord1, int32_t iStatWord1, int32_t iCmdWord2, int32_t iStatWord2, uint16_t auData[]);
    void AppendMsg(ClCh10Format_1553 * psu1553Msg);
    void Commit();

    };

