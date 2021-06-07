

//#include <cstdio>
//#include <cassert>
//#include <ctime>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
//#include <string.h>
#include <stdlib.h>
#include <memory>
//#include <math.h>


// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_tmats.h"

using namespace Irig106;

#include "Ch10Format_PCM.h"

// ----------------------------------------------------------------------------
// ClCh10Format_PCM_SynthFmt1 - PCM Synthetic Data Format 1
//
// Constructor / Destructor
// ----------------------------------------------------------------------------

// Construct PCM packet
ClCh10Format_PCM_SynthFmt1::ClCh10Format_PCM_SynthFmt1()
    {
    memset(&suIPH, 0, sizeof(SuPcmF1_IntraPktHeader));

    // Fill in intrapacket header
    suIPH.uMajorFrStatus = 3;
    suIPH.uMinorFrStatus = 3;

    // Fill in PCM data frame
    suPcmFrame.uFrameSync = 0xFE6B2840;
    }

// ----------------------------------------------------------------------------

ClCh10Format_PCM_SynthFmt1::~ClCh10Format_PCM_SynthFmt1()
    {
    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

// Set the relative time counter

void ClCh10Format_PCM_SynthFmt1::SetRTC(int64_t * pullRelTime)
    {
    vLLInt2TimeArray(pullRelTime, suIPH.aubyIntPktTime);
    }


// ----------------------------------------------------------------------------

// Fill in a frame of synthetic PCM Format 1 data from the current sim state

void ClCh10Format_PCM_SynthFmt1::MakeMsg(ClSimState * pclSimState)
    {
    // Fill in PCM data frame
    suPcmFrame;
    }


// ----------------------------------------------------------------------------

std::string ClCh10Format_PCM_SynthFmt1::TMATS(int & iDIndex, int & iCIndex, std::string sCDLN)
    {
    return std::string("");
    }

