
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

// Macros to make B and C record generation easier and more succinct.

#if 0
#define B_MEASURAND_1WORD(meas_name, mess_index, meas_index, word_pos, bit_mask)                                        \
    ssTMATS <<                                                                                                          \
        "B-" << TmatsIndex.iBIndex << "\\MN-1-"     << mess_index << "-" << meas_index << ":" << #meas_name << ";\n"    \
        "B-" << TmatsIndex.iBIndex << "\\MT-1-"     << mess_index << "-" << meas_index << ":D;\n"                       \
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-" << mess_index << "-" << meas_index << ":1;\n"                       \
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-"    << mess_index << "-" << meas_index << "-1:" << word_pos << ";\n"    \
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-"    << mess_index << "-" << meas_index << "-1:" << #bit_mask << ";\n"   \
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-"    << mess_index << "-" << meas_index << "-1:MSB;\n";                  \
        meas_index++;

#define B_MEASURAND_2WORD(meas_name, mess_index, meas_index, word_pos_1, bit_mask_1, word_pos_2, bit_mask_2)            \
    ssTMATS <<                                                                                                          \
        "B-" << TmatsIndex.iBIndex << "\\MT-1-"     << mess_index << "-" << meas_index << ":D;\n"                       \
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-" << mess_index << "-" << meas_index << ":2;\n"                       \
        "B-" << TmatsIndex.iBIndex << "\\MN-1-"     << mess_index << "-" << meas_index << ":" << #meas_name << ";\n"    \
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-"    << mess_index << "-" << meas_index << "-1:" << word_pos_1 << ";\n"  \
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-"    << mess_index << "-" << meas_index << "-1:" << #bit_mask_1 << ";\n" \
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-"    << mess_index << "-" << meas_index << "-1:MSB;\n"                   \
        "B-" << TmatsIndex.iBIndex << "\\MFP-1-"    << mess_index << "-" << meas_index << "-1:1;\n"                     \
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-"    << mess_index << "-" << meas_index << "-2:" << word_pos_2 << ";\n"  \
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-"    << mess_index << "-" << meas_index << "-2:" << #bit_mask_2 << ";\n" \
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-"    << mess_index << "-" << meas_index << "-2:MSB;\n"                   \
        "B-" << TmatsIndex.iBIndex << "\\MFP-1-"    << mess_index << "-" << meas_index << "-2:2;\n";                    \
        meas_index++;
#endif

#define MIL1553_FMT_1_NUM_MEASURANDS    13

// Return a string with the TMATS B and C sections for this 1553 data message

std::string ClCh10Format_1553_Nav::TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN)
    {
    int                 iMessageIdx = 1;    // 1553 defined message number counter (only 1 message right now)
    int                 iMeasIdx    = 1;    // 1553 defined measurement number counter
    std::stringstream   ssTMATS;

    ssTMATS <<
// This part needs to be moved to the Ch10Writer_1553 object
        "B-" << TmatsIndex.iBIndex << "\\DLN:" << sCDLN << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\NBS\\N:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\BID-1:0000;\n"
        "B-" << TmatsIndex.iBIndex << "\\BNA-1:" << sCDLN << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\BT-1:1553;\n";
// Move the part before this. The 1553 message index needs to be passed in.

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

#if 1
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

#else
    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-1:Nav Status;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-1-1:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-1-1:0000000001111111;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-1-1:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-2:Latitude;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-2:2;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-2-1:21;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-2-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-2-1:MSB;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-2-2:22;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-2-2:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-2-2:MSB;\n";

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-3:Longitude;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-3:2;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-3-1:23;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-3-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-3-1:MSB;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-3-2:24;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-3-2:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-3-2:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-4:Altitude;\n"
        "B-" << TmatsIndex.iBIndex << "\\MT-1-1-4:D;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-4:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-4-1:25;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-4-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-4-1:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-5:Pitch;\n"
        "B-" << TmatsIndex.iBIndex << "\\MT-1-1-5:D;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-5:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-5-1:11;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-5-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-5-1:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-6:Roll;\n"
        "B-" << TmatsIndex.iBIndex << "\\MT-1-1-6:D;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-6:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-6-1:10;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-6-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-6-1:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-7:True Heading;\n"
        "B-" << TmatsIndex.iBIndex << "\\MT-1-1-7:D;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-7:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-7-1:12;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-7-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-7-1:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-8:Velocity X;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-8:2;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-8-1:3;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-8-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-8-1:MSB;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-8-2:4;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-8-2:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-8-2:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-9:Velocity Y;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-9:2;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-9-1:5;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-9-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-9-1:MSB;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-9-2:6;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-9-2:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-9-2:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-10:Velocity Z;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-10:2;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-10-1:7;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-10-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-10-1:MSB;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-10-2:8;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-10-2:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-10-2:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-11:Accel X;\n"
        "B-" << TmatsIndex.iBIndex << "\\MT-1-1-11:D;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-11:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-11-1:14;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-11-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-11-1:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-12:Accel Y;\n"
        "B-" << TmatsIndex.iBIndex << "\\MT-1-1-12:D;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-12:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-12-1:15;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-12-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-12-1:MSB;\n"

        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-13:Accel Z;\n"
        "B-" << TmatsIndex.iBIndex << "\\MT-1-1-13:D;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-13:1;\n"
        "B-" << TmatsIndex.iBIndex << "\\MWN-1-1-13-1:16;\n"
        "B-" << TmatsIndex.iBIndex << "\\MBM-1-1-13-1:FW;\n"
        "B-" << TmatsIndex.iBIndex << "\\MTO-1-1-13-1:MSB;\n"
        "B-" << TmatsIndex.iBIndex << "\\MN-1-1-14:Ground Speed;\n"
        "B-" << TmatsIndex.iBIndex << "\\NML\\N-1-1-14:1;\n";
#endif

    TmatsIndex.iBIndex++;

    assert(iMeasIdx == MIL1553_FMT_1_NUM_MEASURANDS + 1);

    // Define bus message 1 data conversion
#if 1
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
#else

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Nav Status;\n"
//        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:INT;\n"
//        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:16;\n"
//        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
//        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
//        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:NON;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Latitude;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:32;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:8.38190317E-8;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Longitude;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:32;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:8.38190317E-8;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Altitude;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:16;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:4;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Pitch;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:16;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:5.49316406E-3;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Roll;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:16;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:5.49316406E-3;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:True Heading;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:UNS;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:16;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:5.49316406E-3;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Velocity X;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:32;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:3.814697265625E-06;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Velocity Y;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:32;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:3.814697265625E-06;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Velocity Z;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\BWT\\N:32;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:3.814697265625E-06;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Accel X;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.03125;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Accel Y;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.03125;\n";
    TmatsIndex.iCIndex++;

    ssTMATS <<
        "C-" << TmatsIndex.iCIndex << "\\DCN:Accel Z;\n"
        "C-" << TmatsIndex.iCIndex << "\\MN4:OTH;\n"
        "C-" << TmatsIndex.iCIndex << "\\BFM:TWO;\n"
        "C-" << TmatsIndex.iCIndex << "\\MC\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\MA\\N:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\SR:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\DCT:COE;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO\\N:1;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO1:N;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO:0;\n"
        "C-" << TmatsIndex.iCIndex << "\\CO-1:0.03125;\n";
    TmatsIndex.iCIndex++;  
#endif

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

    return ssTMATS.str();
    } // end TMATS()
