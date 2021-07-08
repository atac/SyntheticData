

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
    assert(sizeof(struct SuPcmFrame_Fmt1) == 160);

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
    suPcmFrame_Fmt1.lLATP   = uint32_t(pclSimState->fState["AC_LAT"] / 180.0 * (double)0x7fffffff);
    suPcmFrame_Fmt1.lLONP   = uint32_t(pclSimState->fState["AC_LON"] / 180.0 * (double)0x7fffffff);
    suPcmFrame_Fmt1.iALT    =      int(pclSimState->fState["AC_ALT"]) + 1000;
    suPcmFrame_Fmt1.fTAS    =    float(pclSimState->fState["AC_TAS"]);
    suPcmFrame_Fmt1.fTH     =    float(pclSimState->fState["AC_TRUE_HDG"]);
    suPcmFrame_Fmt1.fMH     =    float(pclSimState->fState["AC_MAG_HDR"]);
    suPcmFrame_Fmt1.fPTCH   =    float(pclSimState->fState["AC_PITCH"]);
    suPcmFrame_Fmt1.fROLL   =    float(pclSimState->fState["AC_ROLL"]);
    suPcmFrame_Fmt1.fAOAC   =    float(pclSimState->fState["AC_AOA"]);
    suPcmFrame_Fmt1.fVRTG   =    float(pclSimState->fState["AC_ACCEL_DOWN"]);

    // Additional data values from NASA data
    suPcmFrame_Fmt1.fGS     =    float(pclSimState->fState["GS"]);
    suPcmFrame_Fmt1.fIVV    =    float(pclSimState->fState["IVV"]);
    suPcmFrame_Fmt1.fFPAC   =    float(pclSimState->fState["FPAC"]);
    suPcmFrame_Fmt1.fPLA_1  =    float(pclSimState->fState["PLA_1"]);
    suPcmFrame_Fmt1.fPLA_2  =    float(pclSimState->fState["PLA_2"]);
    suPcmFrame_Fmt1.fEGT_1  =    float(pclSimState->fState["EGT_1"]);
    suPcmFrame_Fmt1.fEGT_2  =    float(pclSimState->fState["EGT_2"]);
    suPcmFrame_Fmt1.fOIT_1  =    float(pclSimState->fState["OIT_1"]);
    suPcmFrame_Fmt1.fOIT_2  =    float(pclSimState->fState["OIT_2"]);
    suPcmFrame_Fmt1.fFF_1   =    float(pclSimState->fState["FF_1"]);
    suPcmFrame_Fmt1.fFF_2   =    float(pclSimState->fState["FF_2"]);
    suPcmFrame_Fmt1.fN1_1   =    float(pclSimState->fState["N1_1"]);
    suPcmFrame_Fmt1.fN1_2   =    float(pclSimState->fState["N1_2"]);
    suPcmFrame_Fmt1.fN2_1   =    float(pclSimState->fState["N2_1"]);
    suPcmFrame_Fmt1.fN2_2   =    float(pclSimState->fState["N2_2"]);
    suPcmFrame_Fmt1.fVIB_1  =    float(pclSimState->fState["VIB_1"]);
    suPcmFrame_Fmt1.fVIB_2  =    float(pclSimState->fState["VIB_2"]);
    suPcmFrame_Fmt1.fOIP_1  =    float(pclSimState->fState["OIP_1"]);
    suPcmFrame_Fmt1.fOIP_2  =    float(pclSimState->fState["OIP_2"]);
    suPcmFrame_Fmt1.fAOA1   =    float(pclSimState->fState["AOA1"]);
    suPcmFrame_Fmt1.fAOA2   =    float(pclSimState->fState["AOA2"]);
    suPcmFrame_Fmt1.iWOW    =          pclSimState->fState["WOW"]  == 0.0 ? 0 : 1;
    suPcmFrame_Fmt1.iLGDN   =          pclSimState->fState["LGDN"] == 0.0 ? 0 : 1;
    suPcmFrame_Fmt1.iLGUP   =          pclSimState->fState["LGUP"] == 0.0 ? 0 : 1;
    suPcmFrame_Fmt1.fAIL_1  =    float(pclSimState->fState["AIL_1"]);
    suPcmFrame_Fmt1.fAIL_2  =    float(pclSimState->fState["AIL_2"]);
    suPcmFrame_Fmt1.fELEV_1 =    float(pclSimState->fState["ELEV_1"]);
    suPcmFrame_Fmt1.fELEV_2 =    float(pclSimState->fState["ELEV_2"]);
    suPcmFrame_Fmt1.fRUDD   =    float(pclSimState->fState["RUDD"]);
    suPcmFrame_Fmt1.iCWPC   =      int(pclSimState->fState["CWPC"]);
    suPcmFrame_Fmt1.iCWPF   =      int(pclSimState->fState["CWPF"]);
    suPcmFrame_Fmt1.iCCPC   =      int(pclSimState->fState["CCPC"]);
    suPcmFrame_Fmt1.iCCPF   =      int(pclSimState->fState["CCPF"]);
    suPcmFrame_Fmt1.iRUDP   =      int(pclSimState->fState["RUDP"]);
    suPcmFrame_Fmt1.iFLAP   =      int(pclSimState->fState["FLAP"]);
    }

// ----------------------------------------------------------------------------

std::string ClCh10Format_PCM_SynthFmt1::TMATS(int & iDIndex, int & iCIndex, std::string sCDLN)
    {
    return std::string("");
    }


