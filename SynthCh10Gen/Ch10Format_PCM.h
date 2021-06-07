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
#include "i106_decode_pcmf1.h"

#include "SimState.h"

using namespace Irig106;


// ----------------------------------------------------------------------------
// ClCh10Format_PCM_SynthFmt1 - PCM Synthetic Data Format 1
// ----------------------------------------------------------------------------

class ClCh10Format_PCM_SynthFmt1
    {
public:
    ClCh10Format_PCM_SynthFmt1();
    ~ClCh10Format_PCM_SynthFmt1();

    // Data structure for PCM data frame. Note that there are not multiple
    // minor subframes in the overall structure. There is only one major frame.
    // That also implies there is no minor subframe ID in the data.
    struct SuPcmFrame
        {
        uint32_t        uFrameSync;
        
        }; // end SuPcmFrame


    // Class variables
public:
    SuPcmF1_IntraPktHeader  suIPH;
    SuPcmFrame              suPcmFrame;
    const unsigned long     ulDataLen = sizeof(SuPcmFrame);

    // Methods
public:
    void SetRTC(int64_t * pullRelTime);

    virtual void MakeMsg(ClSimState * pclSimState);
    virtual std::string TMATS(int & iDIndex, int & iCIndex, std::string sCDLN);

    };

