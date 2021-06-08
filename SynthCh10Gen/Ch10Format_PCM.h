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
    ClCh10Format_PCM_SynthFmt1(float fFrameRate);
    ~ClCh10Format_PCM_SynthFmt1();

    // Data structure for PCM data frame. Note that there are not multiple
    // minor subframes in the overall structure. There is only one major frame.
    // That also implies there is no minor subframe ID in the data.
#pragma pack(push,1)
    struct SuPcmFrame_Fmt1      // Synthetic Data PCM Frame Data Layout 1
        {
        uint32_t    uFrameSync;
                                // Units      Description
        uint32_t    lLATP;      // SemiCir    LATITUDE POSITION
        uint32_t    lLONP;      // SemiCir    LONGITUDE POSITION
        int16_t     iALT;       // FEET       PRESSURE ALTITUDE

        float       fTAS;       // KNOTS      TRUE AIRSPEED
        float       fTH;        // DEG        TRUE HEADING (+/-180, 0 = north)
        float       fMH;        // DEG        MAGNETIC HEADING
        float       fPTCH;      // DEG        PITCH ANGLE
        float       fROLL;      // DEG        ROLL ANGLE (+ = left wing high)
        float       fAOAC;      // DEG        CORRECTED ANGLE OF ATTACK
        float       fVRTG;      // G          VERTICAL ACCELERATION (+ = down, nominal = +1)
        float       fGS;        // KNOTS      GROUND SPEED
        float       fIVV;       // FT/MIN     INERTIAL VERTICAL SPEED
        float       fFPAC;      // G          FLIGHT PATH ACCELERATION (X,Y component, + = forward)

        float       fPLA_1;     // DEG        POWER LEVER ANGLE 1 (roughly 0 to 90)
        float       fPLA_2;     // DEG        POWER LEVER ANGLE 1 (roughly 0 to 90)
        float       fEGT_1;     // DEG        EXHAUST GAS TEMPERATURE 1
        float       fEGT_2;     // DEG        EXHAUST GAS TEMPERATURE 2
        float       fOIT_1;     // DEG        OIL TEMPERATURE 1
        float       fOIT_2;     // DEG        OIL TEMPERATURE 2
        float       fFF_1;      // LBS/HR     FUEL FLOW 1
        float       fFF_2;      // LBS/HR     FUEL FLOW 2
        float       fN1_1;      // %RPM       FAN SPEED 1
        float       fN1_2;      // %RPM       FAN SPEED 2
        float       fN2_1;      // %RPM       CORE SPEED 1
        float       fN2_2;      // %RPM       CORE SPEED 2
        float       fVIB_1;     // IN/SEC     ENGINE VIBRATION 1
        float       fVIB_2;     // IN/SEC     ENGINE VIBRATION 2
        float       fOIP_1;     // PSI        OIL PRESSURE 1
        float       fOIP_2;     // PSI        OIL PRESSURE 2
        float       fAOA1;      // DEG        ANGLE OF ATTACK 1
        float       fAOA2;      // DEG        ANGLE OF ATTACK 2

        uint16_t    iWOW  : 1;  //            WEIGHT ON WHEELS (0 = true)
        uint16_t    iLGDN : 1;  //            GEARS L&R DOWN LOCKED (0 = true)
        uint16_t    iLGUP : 1;  //            GEARS L&R UP LOCKED (0 = true)
        uint16_t          : 0;

        float       fAIL_1;     // DEG        AILERON POSITION LH
        float       fAIL_2;     // DEG        AILERON POSITION RH
        float       fELEV_1;    // DEG        ELEVATOR POSITION LEFT
        float       fELEV_2;    // DEG        ELEVATOR POSITION RIGHT
        float       fRUDD;      // DEG        RUDDER POSITION

        int16_t     iCWPC;      // COUNTS     CONTROL WHEEL POSITION CAPT
        int16_t     iCWPF;      // COUNTS     CONTROL WHEEL POSITION F/O
        int16_t     iCCPC;      // COUNTS     CONTROL COLUMN POSITION CAPT (750 - 4000)
        int16_t     iCCPF;      // COUNTS     CONTROL COLUMN POSITION F/O
        int16_t     iRUDP;      // COUNTS     RUDDER PEDAL POSITION (1000 - 3000)
        int16_t     iFLAP;      // COUNTS     T.E. FLAP POSITION
        }; // end SuPcmFrame
#pragma pack(pop)


    // Class variables
public:
    SuPcmF1_IntraPktHeader  suIPH;
    SuPcmFrame_Fmt1         suPcmFrame_Fmt1;
    unsigned                uWordLen;       // Word length in bits
    unsigned                uFrameLen;      // PCM frame length in bytes
    float                   fFrameRate;     // Frame rate in Hz

    // Methods
public:
    void SetRTC(int64_t * pullRelTime);
    virtual void MakeMsg(ClSimState * pclSimState);
    virtual std::string TMATS(int & iDIndex, int & iCIndex, std::string sCDLN);

    };

