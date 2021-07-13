

//#include <cstdio>
#include <cassert>
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
ClCh10Format_PCM_SynthFmt1::ClCh10Format_PCM_SynthFmt1(float fFrameRate)
    {
    // Make sure the size of the PCM frame is still correct
    assert(sizeof(struct SuPcmFrame_Fmt1) == 100);

    this->uWordLen   = 16;  // bits
    this->uIPHLen    = 10;  // bytes
    this->uFrameLen  = sizeof(SuPcmFrame_Fmt1);
    this->fFrameRate = fFrameRate;

    memset(&suIPH, 0, sizeof(SuPcmF1_IntraPktHeader));

    // Init in intrapacket header
    suIPH.uMajorFrStatus = 3;
    suIPH.uMinorFrStatus = 3;

    // Init the PCM data frame
    memset(&suPcmFrame_Fmt1, 0, sizeof(struct SuPcmFrame_Fmt1));
    suPcmFrame_Fmt1.uFrameSync = 0x2840FE6B;    // Sync word swapped
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
    // Standard nav data source values that should be in every simulation
    suPcmFrame_Fmt1.lLATP   =  int32_t(FLOAT2SEMICIR32(pclSimState->fState["AC_LAT"]));
    suPcmFrame_Fmt1.lLONP   =  int32_t(FLOAT2SEMICIR32(pclSimState->fState["AC_LON"]));
    suPcmFrame_Fmt1.uALT    = uint16_t(pclSimState->fState["AC_ALT"]) + 1000;
    suPcmFrame_Fmt1.uTAS    = uint16_t(pclSimState->fState["AC_TAS"]);
    suPcmFrame_Fmt1.uTH     = uint16_t(FLOAT2SEMICIR16(pclSimState->fState["AC_TRUE_HDG"]));
    suPcmFrame_Fmt1.uMH     = uint16_t(FLOAT2SEMICIR16(pclSimState->fState["AC_MAG_HDR"]));
    suPcmFrame_Fmt1.iPTCH   =  int16_t(FLOAT2SEMICIR16(pclSimState->fState["AC_PITCH"]));
    suPcmFrame_Fmt1.iROLL   =  int16_t(FLOAT2SEMICIR16(pclSimState->fState["AC_ROLL"]));
    suPcmFrame_Fmt1.iAOAC   =  int16_t(FLOAT2SEMICIR16(pclSimState->fState["AC_AOA"]));
    suPcmFrame_Fmt1.iVRTG   =  int16_t(pclSimState->fState["AC_ACCEL_DOWN"]);

    // Additional data values from NASA data
    suPcmFrame_Fmt1.uGS     = uint16_t(pclSimState->fState["GS"]);
    suPcmFrame_Fmt1.iIVV    =  int16_t(pclSimState->fState["IVV"]);
    suPcmFrame_Fmt1.iFPAC   =  int16_t(pclSimState->fState["FPAC"]);
    suPcmFrame_Fmt1.iPLA_1  =  int16_t(pclSimState->fState["PLA_1"]);
    suPcmFrame_Fmt1.iPLA_2  =  int16_t(pclSimState->fState["PLA_2"]);
    suPcmFrame_Fmt1.iEGT_1  =  int16_t(pclSimState->fState["EGT_1"]);
    suPcmFrame_Fmt1.iEGT_2  =  int16_t(pclSimState->fState["EGT_2"]);
    suPcmFrame_Fmt1.iOIT_1  =  int16_t(pclSimState->fState["OIT_1"]);
    suPcmFrame_Fmt1.iOIT_2  =  int16_t(pclSimState->fState["OIT_2"]);
    suPcmFrame_Fmt1.uFF_1   = uint16_t(pclSimState->fState["FF_1"]);
    suPcmFrame_Fmt1.uFF_2   = uint16_t(pclSimState->fState["FF_2"]);
    suPcmFrame_Fmt1.iN1_1   =  int16_t(pclSimState->fState["N1_1"]);
    suPcmFrame_Fmt1.iN1_2   =  int16_t(pclSimState->fState["N1_2"]);
    suPcmFrame_Fmt1.iN2_1   =  int16_t(pclSimState->fState["N2_1"]);
    suPcmFrame_Fmt1.iN2_2   =  int16_t(pclSimState->fState["N2_2"]);
    suPcmFrame_Fmt1.iVIB_1  =  int16_t(pclSimState->fState["VIB_1"]);
    suPcmFrame_Fmt1.iVIB_2  =  int16_t(pclSimState->fState["VIB_2"]);
    suPcmFrame_Fmt1.iOIP_1  =  int16_t(pclSimState->fState["OIP_1"]);
    suPcmFrame_Fmt1.iOIP_2  =  int16_t(pclSimState->fState["OIP_2"]);
    suPcmFrame_Fmt1.iAOA1   =  int16_t(pclSimState->fState["AOA1"]);
    suPcmFrame_Fmt1.iAOA2   =  int16_t(pclSimState->fState["AOA2"]);
    suPcmFrame_Fmt1.bWOW    =          pclSimState->fState["WOW"]  == 0.0 ? 0 : 1;
    suPcmFrame_Fmt1.bLGDN   =          pclSimState->fState["LGDN"] == 0.0 ? 0 : 1;
    suPcmFrame_Fmt1.bLGUP   =          pclSimState->fState["LGUP"] == 0.0 ? 0 : 1;
    suPcmFrame_Fmt1.iAIL_1  =  int16_t(pclSimState->fState["AIL_1"]);
    suPcmFrame_Fmt1.iAIL_2  =  int16_t(pclSimState->fState["AIL_2"]);
    suPcmFrame_Fmt1.iELEV_1 =  int16_t(pclSimState->fState["ELEV_1"]);
    suPcmFrame_Fmt1.iELEV_2 =  int16_t(pclSimState->fState["ELEV_2"]);
    suPcmFrame_Fmt1.iRUDD   =  int16_t(pclSimState->fState["RUDD"]);
    suPcmFrame_Fmt1.uCWPC   = uint16_t(pclSimState->fState["CWPC"]);
    suPcmFrame_Fmt1.uCWPF   = uint16_t(pclSimState->fState["CWPF"]);
    suPcmFrame_Fmt1.uCCPC   = uint16_t(pclSimState->fState["CCPC"]);
    suPcmFrame_Fmt1.uCCPF   = uint16_t(pclSimState->fState["CCPF"]);
    suPcmFrame_Fmt1.uRUDP   = uint16_t(pclSimState->fState["RUDP"]);
    suPcmFrame_Fmt1.uFLAP   = uint16_t(pclSimState->fState["FLAP"]);
    }

// ----------------------------------------------------------------------------

std::string ClCh10Format_PCM_SynthFmt1::TMATS(int & iDIndex, int & iCIndex, std::string sCDLN)
    {
    return std::string("");
    }


