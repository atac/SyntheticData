#pragma once

//#include <iostream>
#include <vector>

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_arinc429.h"

#include "Common.h"
#include "SimState.h"

using namespace Irig106;


// ----------------------------------------------------------------------------
// ClCh10Format_ARINC429 - ARINC 429 Synthetic Data base class
// ----------------------------------------------------------------------------

class ClCh10Format_ARINC429
    {
public:
    ClCh10Format_ARINC429();
    ~ClCh10Format_ARINC429();

    // This is the relative time stamp of the first 429 message in a packet. This
    // value is put int the Ch 10 header. It is also used to calculate gap times
    // for each 429 message.
    int64_t     llRefTime;

    // Class variables
    unsigned                    uSubchannels;
    
public:
    struct SuArinc429Msg
        {
        SuArinc429F0_Header     suIPH;
        SuArinc429F0_Data       suData;
        };

    std::vector<SuArinc429Msg>  aArinc429Msgs;

    // Methods
public:
    void SetRTC(int64_t * pllRelTime);
    virtual void MakeMsg(ClSimState * pclSimState) = 0;
    virtual std::string TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN) = 0;
    int CalcParity(SuArinc429F0_Data * psuA429Data);
    unsigned char ReverseLabel(unsigned char uLabel);

    };

