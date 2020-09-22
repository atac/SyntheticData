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
#include <vector>

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_index.h"

using namespace Irig106;

class ClCh10Writer_Index
    {
    public:
        ClCh10Writer_Index();
        ~ClCh10Writer_Index();

    // Structures
    uint8_t                         uSeqNum;
    std::vector<SuIndex_NodeMsg>    asuNodeData;
    std::vector<SuIndex_RootMsg>    asuRootData;
    SuIndex_RootMsg                 suPrevRootPacket;

    // Data
public:
    int                 iHandle;
    unsigned int        uChanID;

    // Methods
    void Init(int iHandle, unsigned int uChanID);
    void AppendNodeIndex(SuI106Ch10Header * psuHeader);
    void WriteNodePacket();
    void AppendRootIndex(SuIndex_Time * psuNodeIndexTime, __int64 llNodeIndexOffset);
    void WriteRootPacket();

    };

