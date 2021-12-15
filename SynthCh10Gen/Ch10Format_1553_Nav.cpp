
#include <assert.h>
#include <iostream>     // std::cout
#include <iomanip>

#include "Common.h"
#include "Ch10Format_1553_Nav.h"

#if !defined(__GNUC__)
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#endif

using namespace std;


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

// Construct Transmit, Receive, and Mode Code message
ClCh10Format_1553_Nav::ClCh10Format_1553_Nav(unsigned uRT, unsigned bTR, unsigned uSubAddr, unsigned uWC) :
        ClCh10Format_1553(uRT, bTR, uSubAddr, uWC)
    {
    psuInsData = (SuINS_Data *)&auData;
    }

// ----------------------------------------------------------------------------

// Construct RT to RT message
ClCh10Format_1553_Nav::ClCh10Format_1553_Nav(unsigned uRT_Send,unsigned uRT_Rcv, unsigned uSubAddr_Send, unsigned uSubAddr_Rcv, unsigned uWC) :
        ClCh10Format_1553(uRT_Send, uRT_Rcv, uSubAddr_Send, uSubAddr_Rcv, uWC)
    {
    psuInsData = (SuINS_Data *)&auData;
    }

// ----------------------------------------------------------------------------

ClCh10Format_1553_Nav::~ClCh10Format_1553_Nav()
    {
    }

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void ClCh10Format_1553_Nav::MakeMsg(ClSimState * pclSimState)
    {
    int32_t     lTempVel;
    uint32_t    ulTempLatLon;

    // Zero everthing out
    memset(psuInsData, 0, sizeof(SuINS_Data));

    psuInsData->uStatus      = 0x0077;  // Acceleration data for the NASA set is hosed
    //psuInsData->uTimeTag
    lTempVel = ( int32_t)(pclSimState->fState["AC_VEL_NORTH"] * 0x00040000);
    psuInsData->sVelX_MSW    = ( int16_t)((lTempVel >> 16) & 0x0000ffff);
    psuInsData->uVelX_LSW    = (uint16_t)((lTempVel      ) & 0x0000ffff);
    lTempVel = (int32_t)(pclSimState->fState["AC_VEL_EAST"] * 0x00040000);
    psuInsData->sVelY_MSW    = ( int16_t)((lTempVel >> 16) & 0x0000ffff);
    psuInsData->uVelY_LSW    = (uint16_t)((lTempVel      ) & 0x0000ffff);
    lTempVel = (int32_t)(pclSimState->fState["AC_VEL_DOWN"] * 0x00040000);
    psuInsData->sVelZ_MSW    = ( int16_t)((lTempVel >> 16) & 0x0000ffff);
    psuInsData->uVelZ_LSW    = (uint16_t)((lTempVel      ) & 0x0000ffff);
    psuInsData->uAz          = (uint16_t)(pclSimState->fState["AC_TRUE_HDG"]  / 180.0 * (double)0x7fff);
    psuInsData->sRoll        = ( int16_t)(pclSimState->fState["AC_ROLL"]      / 180.0 * (double)0x7fff);
    psuInsData->sPitch       = ( int16_t)(pclSimState->fState["AC_PITCH"]     / 180.0 * (double)0x7fff);
    psuInsData->uTrueHeading = (uint16_t)(pclSimState->fState["AC_TRUE_HDG"]  / 180.0 * (double)0x7fff);
    psuInsData->uMagHeading  = (uint16_t)(pclSimState->fState["AC_MAG_HDG"]   / 180.0 * (double)0x7fff);
    psuInsData->sAccX        = ( int16_t)(pclSimState->fState["AC_ACCEL_NORTH"] * 32.0);
    psuInsData->sAccY        = ( int16_t)(pclSimState->fState["AC_ACCEL_EAST"]  * 32.0);
    psuInsData->sAccZ        = ( int16_t)(pclSimState->fState["AC_ACCEL_DOWN"]  * 32.0);
    //psuInsData->sCXX_MSW
    //psuInsData->uCXX_LSW
    //psuInsData->sCXY_MSW
    //psuInsData->uCXY_LSW
    ulTempLatLon = (uint32_t)(pclSimState->fState["AC_LAT"] / 180.0 * (double)0x7fffffff);
    psuInsData->sLat_MSW     = ((ulTempLatLon >> 16) & 0x0000ffff);
    psuInsData->uLat_LSW     = ( ulTempLatLon        & 0x0000ffff);
    ulTempLatLon = (uint32_t)(pclSimState->fState["AC_LON"] / 180.0 * (double)0x7fffffff);
    psuInsData->sLon_MSW     = ((ulTempLatLon >> 16) & 0x0000ffff);
    psuInsData->uLon_LSW     = ( ulTempLatLon        & 0x0000ffff);
    psuInsData->sAlt         = (int16_t)(pclSimState->fState["AC_ALT"] / 4.0);
    //psuInsData->sSteeringError
    //psuInsData->sTiltX
    //psuInsData->sTiltY
    //psuInsData->sJustInCase[4]
    }


// ----------------------------------------------------------------------------

#define MIL1553_FMT_1_NUM_MEASURANDS    13

// Return a string with the TMATS B and C sections for this 1553 data message

std::string ClCh10Format_1553_Nav::TMATS(ClTmatsIndexes & TmatsIndex)
    {
    int                 iMessageIdx = 1;    // 1553 defined message number counter (only 1 message right now)
    int                 iMeasIdx    = 1;    // 1553 defined measurement number counter
    std::stringstream   ssTMATS;

    // Bus Message 1 - NAV
    // --------------------------

    // Define bus message 1
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\NMS\\N-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MID-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MNA-1-1:37A0 INS;\n"
        "B-" << TmatsIndex.iBIndex << "\\TRA-1-1:00110;\n"
        "B-" << TmatsIndex.iBIndex << "\\STA-1-1:11101;\n"
        "B-" << TmatsIndex.iBIndex << "\\TRM-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\DWC-1-1:00000;\n";

    // Define bus message 1 measurands
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN\\N-1-1:" << MIL1553_FMT_1_NUM_MEASURANDS << ";\n";

    B_MEASURAND_1WORD(BUS_NAV_STATUS,   iMessageIdx, iMeasIdx,  1, 0000000001111111);
    B_MEASURAND_2WORD(BUS_LATITUDE,     iMessageIdx, iMeasIdx, 21, FW, 22, FW);
    B_MEASURAND_2WORD(BUS_LONGITUDE,    iMessageIdx, iMeasIdx, 23, FW, 24, FW);
    B_MEASURAND_1WORD(BUS_ALTITUDE,     iMessageIdx, iMeasIdx, 25, FW);
    B_MEASURAND_1WORD(BUS_PITCH,        iMessageIdx, iMeasIdx, 11, FW);
    B_MEASURAND_1WORD(BUS_ROLL,         iMessageIdx, iMeasIdx, 10, FW);
    B_MEASURAND_1WORD(BUS_TRUE_HEADING, iMessageIdx, iMeasIdx, 12, FW);
    B_MEASURAND_2WORD(BUS_VELOCITY_X,   iMessageIdx, iMeasIdx,  3, FW,  4, FW);
    B_MEASURAND_2WORD(BUS_VELOCITY_Y,   iMessageIdx, iMeasIdx,  5, FW,  6, FW);
    B_MEASURAND_2WORD(BUS_VELOCITY_Z,   iMessageIdx, iMeasIdx,  7, FW,  8, FW);
    B_MEASURAND_1WORD(BUS_ACCEL_X,      iMessageIdx, iMeasIdx, 14, FW);
    B_MEASURAND_1WORD(BUS_ACCEL_Y,      iMessageIdx, iMeasIdx, 15, FW);
    B_MEASURAND_1WORD(BUS_ACCEL_Z,      iMessageIdx, iMeasIdx, 16, FW);

    assert(iMeasIdx == MIL1553_FMT_1_NUM_MEASURANDS + 1);

    // Define bus message 1 data conversion
    C_CONVERSION_NONE        (BUS_NAV_STATUS, Bus Nav Status, Bool, INT)
    C_CONVERSION_OFFSET_SCALE(BUS_LATITUDE,     Latitude,      Deg, TWO, 0.0, 8.38190317E-8)
    C_CONVERSION_OFFSET_SCALE(BUS_LONGITUDE,    Longitude,     Deg, TWO, 0.0, 8.38190317E-8)
    C_CONVERSION_OFFSET_SCALE(BUS_ALTITUDE,     Altitude,       Ft, TWO, 0.0, 4.0)
    C_CONVERSION_OFFSET_SCALE(BUS_PITCH,        Pitch Angle,   Deg, TWO, 0.0, 5.49316406E-3)
    C_CONVERSION_OFFSET_SCALE(BUS_ROLL,         Roll Angle,    Deg, TWO, 0.0, 5.49316406E-3)
    C_CONVERSION_OFFSET_SCALE(BUS_TRUE_HEADING, True Heading,  Deg, UNS, 0.0, 5.49316406E-3)
    C_CONVERSION_OFFSET_SCALE(BUS_VELOCITY_X,   Velocity X, ft/sec, TWO, 0.0, 3.814697265625E-06)
    C_CONVERSION_OFFSET_SCALE(BUS_VELOCITY_Y,   Velocity Y, ft/sec, TWO, 0.0, 3.814697265625E-06)
    C_CONVERSION_OFFSET_SCALE(BUS_VELOCITY_Z,   Velocity Z, ft/sec, TWO, 0.0, 3.814697265625E-06)
    C_CONVERSION_OFFSET_SCALE(BUS_ACCEL_X,      Accel X,  ft/sec^2, TWO, 0.0, 0.03125)
    C_CONVERSION_OFFSET_SCALE(BUS_ACCEL_Y,      Accel Y,  ft/sec^2, TWO, 0.0, 0.03125)
    C_CONVERSION_OFFSET_SCALE(BUS_ACCEL_Z,      Accel Z,  ft/sec^2, TWO, 0.0, 0.03125)

#if 0
    // No derived calculations for now. Nobody can seem to agree on how it is implemented.
    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Ground Speed;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN1:Calculated Ground Speed;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN3:kts;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:FPT;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:DER;\n"
        "C-" << TmatsIndex.iCIndex << "\\DPAT:A;\n"
        "C-" << TmatsIndex.iCIndex << "\\DPA:Sqrt(a*a+b*b)*3600/6076;\n"
        "C-" << TmatsIndex.iCIndex << "\\DPTM:BUS_VELOCITY_Y;\n"
        "C-" << TmatsIndex.iCIndex << "\\DPNO:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\DP\\N:2;\n"
        "C-" << TmatsIndex.iCIndex << "\\DP-1:BUS_VELOCITY_X;\n"
        "C-" << TmatsIndex.iCIndex << "\\DP-2:BUS_VELOCITY_Y;\n";
    TmatsIndex.iCIndex++;
#endif

    return ssTMATS.str();
    } // end TMATS()
