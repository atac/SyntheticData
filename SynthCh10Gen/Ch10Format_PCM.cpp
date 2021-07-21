

//#include <cstdio>
#include <cassert>
//#include <ctime>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <iomanip>
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

using namespace std;
using namespace Irig106;

#include "Common.h"
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

// A couple of macros to make D and C record generation easier and more succinct.

#define D_MEASURAND_1WORD(meas_name, meas_index, word_pos, bit_mask)                                \
    ssTMATS <<                                                                                      \
        "D-" << TmatsIndex.iDIndex << "\\MN-1-" << meas_index << ":" << #meas_name << ";\n"         \
        "D-" << TmatsIndex.iDIndex << "\\LT-1-" << meas_index << ":WDFR;\n"                         \
        "D-" << TmatsIndex.iDIndex << "\\MML\\N-1-" << meas_index << ":1;\n"                        \
        /* Measurand XXX Location 1; */                                                             \
        "D-" << TmatsIndex.iDIndex << "\\MNF\\N-1-" << meas_index << "-1:1;\n"                      \
        /* Measurand XXX Location 1 Word 1; */                                                      \
        "D-" << TmatsIndex.iDIndex << "\\WP-1-" << meas_index << "-1-1:" << word_pos << ";\n"       \
        "D-" << TmatsIndex.iDIndex << "\\WFM-1-" << meas_index << "-1-1:" << #bit_mask << ";\n"     \
        "D-" << TmatsIndex.iDIndex << "\\WFP-1-" << meas_index << "-1-1:1;\n";                      \
        meas_index++;

#define PCM_FMT_1_NUM_MEASURANDS    45

#define C_CONVERSION(meas_name, conv_name, units, bin_fmt, offset, scale)   \
    ssTMATS <<                                                              \
        "C-" << TmatsIndex.iCIndex << "\\DCN:" << #meas_name << ";\n"       \
        "C-" << TmatsIndex.iCIndex << "\\MN1:" << #conv_name << ";\n"       \
        "C-" << TmatsIndex.iCIndex << "\\MN3:" << #units << ";\n"           \
        "C-" << TmatsIndex.iCIndex << "\\BFM:" << #bin_fmt << ";\n"         \
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"                        \
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"                        \
        "C-" << TmatsIndex.iCIndex << "\\CO:" << (offset) << ";\n"          \
        "C-" << TmatsIndex.iCIndex << "\\CO-1:" << uppercase << setprecision(12) << (scale) << ";\n";  \
    TmatsIndex.iCIndex++;


std::string ClCh10Format_PCM_SynthFmt1::TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN)
    {
    std::stringstream   ssTMATS;
    unsigned long       ulDataRate;
    unsigned            uWordsPerMinorFrame;
    unsigned            uBitsPerMinorFrame;
    int                 iMeasIdx;

    // Calculate some parameters
    uBitsPerMinorFrame  = uFrameLen * 8;
    uWordsPerMinorFrame = ((uBitsPerMinorFrame - 32) / uWordLen) + 1;
    ulDataRate          = unsigned long(fFrameRate * float(uBitsPerMinorFrame));
    
    // PCM attributes specific to Synthetic PCM Data Format 1
    ssTMATS <<
        "P-" << TmatsIndex.iPIndex << "\\DLN:" << sCDLN << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\D1:NRZ-L;\n"
        "P-" << TmatsIndex.iPIndex << "\\D2:" << ulDataRate << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\D3:U;\n"
        "P-" << TmatsIndex.iPIndex << "\\D4:N;\n"
        "P-" << TmatsIndex.iPIndex << "\\D5:N;\n"
        "P-" << TmatsIndex.iPIndex << "\\D6:N;\n"
        "P-" << TmatsIndex.iPIndex << "\\D7:N;\n"
        "P-" << TmatsIndex.iPIndex << "\\D8:N/A;\n"
        "P-" << TmatsIndex.iPIndex << "\\TF:ONE;\n"
        "P-" << TmatsIndex.iPIndex << "\\F1:" << uWordLen << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\F2:M;\n"
        "P-" << TmatsIndex.iPIndex << "\\F3:NO;\n"
        "P-" << TmatsIndex.iPIndex << "\\MF\\N:1;\n"
        "P-" << TmatsIndex.iPIndex << "\\MF1:" << uWordsPerMinorFrame << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\MF2:" << uBitsPerMinorFrame  << ";\n"
        "P-" << TmatsIndex.iPIndex << "\\MF3:FPT;\n"
        "P-" << TmatsIndex.iPIndex << "\\MF4:32;\n"
        "P-" << TmatsIndex.iPIndex << "\\MF5:11111110011010110010100001000000;\n"
        "P-" << TmatsIndex.iPIndex << "\\SYNC1:2;\n"
        "P-" << TmatsIndex.iPIndex << "\\SYNC2:0;\n"
        "P-" << TmatsIndex.iPIndex << "\\SYNC3:2;\n"
        "P-" << TmatsIndex.iPIndex << "\\SYNC4:0;\n"
        "P-" << TmatsIndex.iPIndex << "\\ISF\\N:0;\n";
    TmatsIndex.iPIndex++;

    // PCM Measurement Description (D)
    // -------------------------------

    // This stuff is confusing so I break down the first couple in some detail

    ssTMATS <<
        "D-" << TmatsIndex.iDIndex << "\\DLN:" << sCDLN << ";\n"
        "D-" << TmatsIndex.iDIndex << "\\ML\\N:1;\n";

    ssTMATS <<
    // Measurement List 1 - Synthetic PCM Data Format 1;
        "D-" << TmatsIndex.iDIndex << "\\MLN-1:SynthPcmFormat1;\n"
        //                                   ^--- Measurement List Number 1
        "D-" << TmatsIndex.iDIndex << "\\MN\\N-1:" << PCM_FMT_1_NUM_MEASURANDS << ";\n";

    ssTMATS <<
    // Measurand - Latitude;
        "D-" << TmatsIndex.iDIndex << "\\MN-1-1:PCM_Latitude;\n"
        //                                    ^--- Measurand 1
        //                                  ^--- Measurement List Number 1
        "D-" << TmatsIndex.iDIndex << "\\LT-1-1:WDFR;\n"
        "D-" << TmatsIndex.iDIndex << "\\MML\\N-1-1:1;\n"
        //                                          ^--- Location Definitions
        //                                        ^--- Measurand 1
        //                                      ^--- Measurement List Number 1

        // Measurand 1 Location 1;
        "D-" << TmatsIndex.iDIndex << "\\MNF\\N-1-1-1:2;\n"
        //                                            ^--- Number of Words in this Fragment
        //                                          ^--- Defined Location 1 of 1
        // Measurand 1 Location 1 Word 1;
        "D-" << TmatsIndex.iDIndex << "\\WP-1-1-1-1:1;\n"
        //                                          ^--- Word Location
        //                                        ^--- Word 1 of 2
        "D-" << TmatsIndex.iDIndex << "\\WFM-1-1-1-1:FW;\n"
        "D-" << TmatsIndex.iDIndex << "\\WFP-1-1-1-1:1;\n"
        // Measurand 1 Location 1 Word 2;
        "D-" << TmatsIndex.iDIndex << "\\WP-1-1-1-2:2;\n"
        //                                          ^--- Word Location
        //                                        ^--- Word 2 of 1
        "D-" << TmatsIndex.iDIndex << "\\WFM-1-1-1-2:FW;\n"
        "D-" << TmatsIndex.iDIndex << "\\WFP-1-1-1-2:2;\n";

    ssTMATS <<
        // Measurand - Longitude;
        "D-" << TmatsIndex.iDIndex << "\\MN-1-2:PCM_Longitude;\n"
        "D-" << TmatsIndex.iDIndex << "\\LT-1-2:WDFR;\n"
        "D-" << TmatsIndex.iDIndex << "\\D-1\\MML\\N-1-2:1;\n"
        // Measurand 2 Location 1;
        "D-" << TmatsIndex.iDIndex << "\\D-1\\MNF\\N-1-2-1:2;\n"
        // Measurand 2 Location 1 Word 1;
        "D-" << TmatsIndex.iDIndex << "\\WP-1-2-1-1:3;\n"
        "D-" << TmatsIndex.iDIndex << "\\WFM-1-2-1-1:FW;\n"
        "D-" << TmatsIndex.iDIndex << "\\WFP-1-2-1-1:1;\n"
        // Measurand 2 Location 1 Word 2;
        "D-" << TmatsIndex.iDIndex << "\\WP-1-2-1-2:4;\n"
        "D-" << TmatsIndex.iDIndex << "\\WFM-1-2-1-2:FW;\n"
        "D-" << TmatsIndex.iDIndex << "\\WFP-1-2-1-2:2;\n";

    // Set the measurand counter
    iMeasIdx = 3;

    // Measurand - Pressure Altitude;
    D_MEASURAND_1WORD(PCM_Pressure_Altitude, iMeasIdx, 5, FW);

    D_MEASURAND_1WORD(PCM_True_Airspeed,     iMeasIdx, 6,  FW);
    D_MEASURAND_1WORD(PCM_True_Heading,      iMeasIdx, 7,  FW);
    D_MEASURAND_1WORD(PCM_Magnetic_Heading,  iMeasIdx, 8,  FW);
    D_MEASURAND_1WORD(PCM_Pitch,             iMeasIdx, 9, FW);
    D_MEASURAND_1WORD(PCM_Roll,              iMeasIdx, 10, FW);
    D_MEASURAND_1WORD(PCM_AngleOfAttack,     iMeasIdx, 11, FW);
    D_MEASURAND_1WORD(PCM_Vert_Accel,        iMeasIdx, 12, FW);
    D_MEASURAND_1WORD(PCM_Ground_Speed,      iMeasIdx, 13, FW);
    D_MEASURAND_1WORD(PCM_Vertical_Speed,    iMeasIdx, 14, FW);
    D_MEASURAND_1WORD(PCM_Flt_Path_Accel,    iMeasIdx, 15, FW);

    D_MEASURAND_1WORD(PCM_Power_Lever_1,     iMeasIdx, 16, FW);
    D_MEASURAND_1WORD(PCM_Power_Lever_2,     iMeasIdx, 17, FW);
    D_MEASURAND_1WORD(PCM_EGT_1,             iMeasIdx, 18, FW);
    D_MEASURAND_1WORD(PCM_EGT_2,             iMeasIdx, 19, FW);
    D_MEASURAND_1WORD(PCM_Oil_Temp_1,        iMeasIdx, 20, FW);
    D_MEASURAND_1WORD(PCM_Oil_Temp_2,        iMeasIdx, 21, FW);
    D_MEASURAND_1WORD(PCM_Fuel_Flow_1,       iMeasIdx, 22, FW);
    D_MEASURAND_1WORD(PCM_Fuel_Flow_2,       iMeasIdx, 23, FW);
    D_MEASURAND_1WORD(PCM_Fan_Speed_1,       iMeasIdx, 24, FW);
    D_MEASURAND_1WORD(PCM_Fan_Speed_2,       iMeasIdx, 25, FW);
    D_MEASURAND_1WORD(PCM_Core_Speed_1,      iMeasIdx, 26, FW);
    D_MEASURAND_1WORD(PCM_Core_Speed_2,      iMeasIdx, 27, FW);
    D_MEASURAND_1WORD(PCM_Engine_Vib_1,      iMeasIdx, 28, FW);
    D_MEASURAND_1WORD(PCM_Engine_Vib_2,      iMeasIdx, 29, FW);
    D_MEASURAND_1WORD(PCM_Oil_Pressure_1,    iMeasIdx, 30, FW);
    D_MEASURAND_1WORD(PCM_Oil_Pressure_2,    iMeasIdx, 31, FW);
    D_MEASURAND_1WORD(PCM_AngleOfAttack_1,   iMeasIdx, 32, FW);
    D_MEASURAND_1WORD(PCM_AngleOfAttack_2,   iMeasIdx, 33, FW);

    D_MEASURAND_1WORD(PCM_Weigh_On_Wheels,    iMeasIdx, 34, 0000000000000001);
    D_MEASURAND_1WORD(PCM_Gear_Down_Locked,   iMeasIdx, 34, 0000000000000010);
    D_MEASURAND_1WORD(PCM_Gear_Up_Locked,     iMeasIdx, 34, 0000000000000100);

    D_MEASURAND_1WORD(PCM_Aileron_Pos_1,     iMeasIdx, 35, FW);
    D_MEASURAND_1WORD(PCM_Aileron_Pos_2,     iMeasIdx, 36, FW);
    D_MEASURAND_1WORD(PCM_Elevator_Pos_1,    iMeasIdx, 37, FW);
    D_MEASURAND_1WORD(PCM_Elevator_Pos_2,    iMeasIdx, 38, FW);
    D_MEASURAND_1WORD(PCM_Rudder_Pos,        iMeasIdx, 39, FW);

    D_MEASURAND_1WORD(PCM_Control_Wheel_Pos_Capt, iMeasIdx, 40, FW);
    D_MEASURAND_1WORD(PCM_Contorl_Wheel_Pos_FO,   iMeasIdx, 41, FW);
    D_MEASURAND_1WORD(PCM_Control_Col_Pos_Capt,   iMeasIdx, 42, FW);
    D_MEASURAND_1WORD(PCM_Control_Col_Pos_FO,     iMeasIdx, 43, FW);
    D_MEASURAND_1WORD(PCM_Rudder_Pedal_Pos,       iMeasIdx, 44, FW);
    D_MEASURAND_1WORD(PCM_Flap_Pos,               iMeasIdx, 45, FW);

    assert(iMeasIdx == PCM_FMT_1_NUM_MEASURANDS + 1);

    // PCM Measurement Data Conversion (C)
    // -----------------------------------

    C_CONVERSION(PCM_Latitude,          TSPI_LAT,      Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR32(1.0))
    C_CONVERSION(PCM_Longitude,         TSPI_LON,      Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR32(1.0))
    C_CONVERSION(PCM_Pressure_Altitude, TSPI_ALT,      ft,      UNS, -1000.0, 1.0)
    
    C_CONVERSION(PCM_True_Airspeed,     TSPI_AIRSPEED, kts,     ONE, 0.0, 1.0)
    C_CONVERSION(PCM_True_Heading,      TSPI_HEADING,  Deg,     UNS, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_Magnetic_Heading,  MAGNETIC_HEADING, Deg,  UNS, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_Pitch,             TSPI_ROLL,     Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_Roll,              TSPI_PITCH,    Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))

    C_CONVERSION(PCM_AngleOfAttack,   ANGLE_OF_ATTACK, Deg,     TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Vert_Accel,      VERTICAL_ACCEL,  Gs,      TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Ground_Speed,    GROUND_SPEED,    kts,     UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Vertical_Speed,  VERTICAL_SPEED,  kts,     TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Flt_Path_Accel,  FLT_PATH_ACCEL,  Gs,      TWO, 0.0, 1.0)

    C_CONVERSION(PCM_Power_Lever_1,   POWER_LEVER_1,   Deg,     UNS, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_Power_Lever_2,   POWER_LEVER_2,   Deg,     UNS, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_EGT_1,           EGT_1,           DegF,    TWO, 0.0, 1.0)
    C_CONVERSION(PCM_EGT_2,           EGT_2,           DegF,    TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Oil_Temp_1,      OIL_TEMP_1,      DegF,    TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Oil_Temp_2,      OIL_TEMP_2,      DegF,    TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Fuel_Flow_1,     FUEL_FLOW_1,     Lbs/Hr,  UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Fuel_Flow_2,     FUEL_FLOW_2,     Lbs/Hr,  UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Fan_Speed_1,     FAN_SPEED_1,     PCT_RPM, TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Fan_Speed_2,     FAN_SPEED_2,     PCT_RPM, TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Core_Speed_1,    CORE_SPEED_1,    PCT_RPM, TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Core_Speed_2,    CORE_SPEED_2,    PCT_RPM, TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Engine_Vib_1,    ENGINE_VIB_1,    IN/SEC,  TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Engine_Vib_2,    ENGINE_VIB_2,    IN/SEC,  TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Oil_Pressure_1,  OIL_PRESSURE_1,  PSI,     TWO, 0.0, 1.0)
    C_CONVERSION(PCM_Oil_Pressure_2,  OIL_PRESSURE_2,  PSI,     TWO, 0.0, 1.0)
    C_CONVERSION(PCM_AngleOfAttack_1, AOA_SENSOR_1,    Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_AngleOfAttack_2, AOA_SENSOR_2,    Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))

    C_CONVERSION(PCM_Weigh_On_Wheels,  WEIGHT_ON_WHEELS, BOOL,  UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Gear_Down_Locked, GEAR_DOWN_LOCKED, BOOL,  UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Gear_Up_Locked,   GEAR_UP_LOCKED,   BOOL,  UNS, 0.0, 1.0)

    C_CONVERSION(PCM_Aileron_Pos_1,  AILERON_POS_1,    Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_Aileron_Pos_2,  AILERON_POS_2,    Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_Elevator_Pos_1, ELEVATOR_POS_1,   Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_Elevator_Pos_2, ELEVATOR_POS_2,   Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))
    C_CONVERSION(PCM_Rudder_Pos,     RUDDER_POS,       Deg,     TWO, 0.0, 1.0/FLOAT2SEMICIR16(1.0))

    C_CONVERSION(PCM_Control_Wheel_Pos_Capt, CONTROL_WHEEL_POS_CAPT, COUNTS, UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Contorl_Wheel_Pos_FO,   CONTROL_WHEEL_POS_FO,   COUNTS, UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Control_Col_Pos_Capt,   CONTROL_COL_POS_CAPT,   COUNTS, UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Control_Col_Pos_FO,     CONTROL_COL_POS_FO,     COUNTS, UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Rudder_Pedal_Pos,       RUDDER_PEDAL_POS,       COUNTS, UNS, 0.0, 1.0)
    C_CONVERSION(PCM_Flap_Pos,               FLAP_POS,               COUNTS, UNS, 0.0, 1.0)

    return ssTMATS.str();
    }


