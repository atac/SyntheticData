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

#include "Common.h"
#include "SimState.h"

using namespace Irig106;

// Some helper macros for converting back and forth with semi-circles
#define FLOAT2SEMICIR16(angle)  ((angle) * (double)0x4000     / 90.0)
#define FLOAT2SEMICIR32(angle)  ((angle) * (double)0x40000000 / 90.0)
#define SEMICIR162FLOAT(angle)  ((angle) * 90.0 / 0x4000    )
#define SEMICIR322FLOAT(angle)  ((angle) * 90.0 / 0x40000000)


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
        int32_t     lLATP;      // SemiCir    LATITUDE POSITION
        int32_t     lLONP;      // SemiCir    LONGITUDE POSITION
        uint16_t    uALT;       // FEET       PRESSURE ALTITUDE

        uint16_t    uTAS;       // KNOTS      TRUE AIRSPEED
        uint16_t    uTH;        // SemiCir    TRUE HEADING (+/-180, 0 = north)
        uint16_t    uMH;        // SemiCir    MAGNETIC HEADING
        int16_t     iPTCH;      // SemiCir    PITCH ANGLE
        int16_t     iROLL;      // SemiCir    ROLL ANGLE (+ = left wing high)
        int16_t     iAOAC;      // SemiCir    CORRECTED ANGLE OF ATTACK
        int16_t     iVRTG;      // G          VERTICAL ACCELERATION (+ = down, nominal = +1)
        uint16_t    uGS;        // KNOTS      GROUND SPEED
        int16_t     iIVV;       // FT/MIN     INERTIAL VERTICAL SPEED
        int16_t     iFPAC;      // G          FLIGHT PATH ACCELERATION (X,Y component, + = forward)

        int16_t     iPLA_1;     // SemiCir    POWER LEVER ANGLE 1 (roughly 0 to 90)
        int16_t     iPLA_2;     // SemiCir    POWER LEVER ANGLE 1 (roughly 0 to 90)
        int16_t     iEGT_1;     // DEG        EXHAUST GAS TEMPERATURE 1
        int16_t     iEGT_2;     // DEG        EXHAUST GAS TEMPERATURE 2
        int16_t     iOIT_1;     // DEG        OIL TEMPERATURE 1
        int16_t     iOIT_2;     // DEG        OIL TEMPERATURE 2
        uint16_t    uFF_1;      // LBS/HR     FUEL FLOW 1
        uint16_t    uFF_2;      // LBS/HR     FUEL FLOW 2
        int16_t     iN1_1;      // %RPM       FAN SPEED 1
        int16_t     iN1_2;      // %RPM       FAN SPEED 2
        int16_t     iN2_1;      // %RPM       CORE SPEED 1
        int16_t     iN2_2;      // %RPM       CORE SPEED 2
        int16_t     iVIB_1;     // IN/SEC     ENGINE VIBRATION 1
        int16_t     iVIB_2;     // IN/SEC     ENGINE VIBRATION 2
        int16_t     iOIP_1;     // PSI        OIL PRESSURE 1
        int16_t     iOIP_2;     // PSI        OIL PRESSURE 2
        int16_t     iAOA1;      // SemiCir    ANGLE OF ATTACK 1
        int16_t     iAOA2;      // SemiCir    ANGLE OF ATTACK 2

        uint16_t    bWOW  : 1;  //            WEIGHT ON WHEELS (0 = true)
        uint16_t    bLGDN : 1;  //            GEARS L&R DOWN LOCKED (0 = true)
        uint16_t    bLGUP : 1;  //            GEARS L&R UP LOCKED (0 = true)
        uint16_t          : 0;

        int16_t     iAIL_1;     // DEG        AILERON POSITION LH
        int16_t     iAIL_2;     // DEG        AILERON POSITION RH
        int16_t     iELEV_1;    // DEG        ELEVATOR POSITION LEFT
        int16_t     iELEV_2;    // DEG        ELEVATOR POSITION RIGHT
        int16_t     iRUDD;      // DEG        RUDDER POSITION

        uint16_t    uCWPC;      // COUNTS     CONTROL WHEEL POSITION CAPT
        uint16_t    uCWPF;      // COUNTS     CONTROL WHEEL POSITION F/O
        uint16_t    uCCPC;      // COUNTS     CONTROL COLUMN POSITION CAPT (750 - 4000)
        uint16_t    uCCPF;      // COUNTS     CONTROL COLUMN POSITION F/O
        uint16_t    uRUDP;      // COUNTS     RUDDER PEDAL POSITION (1000 - 3000)
        uint16_t    uFLAP;      // COUNTS     T.E. FLAP POSITION
        uint16_t    uFiller[3]; // Fill to 100 bytes
        }; // end SuPcmFrame
#pragma pack(pop)


    // Class variables
public:
    unsigned int            uChanID;
    SuPcmF1_IntraPktHeader  suIPH;
    SuPcmFrame_Fmt1         suPcmFrame_Fmt1;
    unsigned                uWordLen;       // PCM common word length in bits
    unsigned                uIPHLen;        // IPH length in bytes
    unsigned                uFrameLen;      // PCM frame length in bytes
    float                   fFrameRate;     // Frame rate in Hz

    // Methods
public:
    void SetRTC(int64_t * pullRelTime);
    virtual void MakeMsg(ClSimState * pclSimState);
    virtual std::string TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN);

    };

