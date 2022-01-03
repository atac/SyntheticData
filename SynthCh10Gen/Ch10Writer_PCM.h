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
//#include "i106_decode_1553f1.h"
#include "i106_decode_pcmf1.h"
#include "i106_decode_tmats.h"

#include "Common.h"
#include "Ch10Format_PCM.h"

using namespace Irig106;


class ClCh10Writer_PCM
    {
    public:
        ClCh10Writer_PCM();
        ~ClCh10Writer_PCM();

    // Structures
#pragma pack(push, 1)
    // PCM write buffer info (why did I make this struct?)
    struct SuWriteMsgPCM
        {
        SuI106Ch10Header          suCh10Header;
        SuPcmF1_ChanSpec        * psuPCM_CSDW;
        unsigned char           * pchDataBuff;  // Make this char * makes pointer math easier
        uint32_t                  uBuffLen;     // Size of the write buffer
        } suWriteMsgPCM;
#pragma pack(pop)

    // Data
public:
    int                           iHandle;
    unsigned int                  uChanID;
    std::string                   sCDLN;        // Linking Channel Data Link Name for TMATS
    ClCh10Format_PCM_SynthFmt1  * pSynthPcmFmt1;

    // Methods
public:
    void Init(int iHandle, unsigned int uChanID);
    std::string TMATS(ClTmatsIndexes & TmatsIndex, std::string sDescription="");
    void AppendMsg(ClCh10Format_PCM_SynthFmt1 * psuPcmFrame);
    void Commit();

    };

