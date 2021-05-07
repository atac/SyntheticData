#include "Ch10Format_1553_Nav.h"

#if !defined(__GNUC__)
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#endif



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
    psuInsData->sAccX        = (int16_t)pclSimState->fState["AC_ACCEL_NORTH"] * 32.0;
    psuInsData->sAccY        = (int16_t)pclSimState->fState["AC_ACCEL_EAST"]  * 32.0;
    psuInsData->sAccZ        = (int16_t)pclSimState->fState["AC_ACCEL_DOWN"]  * 32.0;
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

// Return a string with the TMATS B and C sections for this 1553 data message

std::string ClCh10Format_1553_Nav::TMATS(int & iBIndex, int & iCIndex, std::string sCDLN)
    {
    std::stringstream   ssTMATS;

    ssTMATS <<
        "B-" << iBIndex << "\\DLN:" << sCDLN << ";\n"
        "B-" << iBIndex << "\\NBS\\N:1;\n"
        "B-" << iBIndex << "\\BID-1:0000;\n"
        "B-" << iBIndex << "\\BNA-1:" << sCDLN << ";\n"
        "B-" << iBIndex << "\\BT-1:1553;\n"
        "B-" << iBIndex << "\\NMS\\N-1:1;\n"
        "B-" << iBIndex << "\\MID-1-1:2;\n"
        "B-" << iBIndex << "\\MNA-1-1:37A0 INS;\n"
        "B-" << iBIndex << "\\TRA-1-1:00110;\n"
        "B-" << iBIndex << "\\STA-1-1:11101;\n"
        "B-" << iBIndex << "\\TRM-1-1:1;\n"
        "B-" << iBIndex << "\\DWC-1-1:00000;\n"
        "B-" << iBIndex << "\\MN\\N-1-1:14;\n"
        "B-" << iBIndex << "\\MN-1-1-1:Nav Status;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-1:1;\n"
        "B-" << iBIndex << "\\MWN-1-1-1-1:1;\n"
        "B-" << iBIndex << "\\MBM-1-1-1-1:0000000001111111;\n"
        "B-" << iBIndex << "\\MTO-1-1-1-1:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-2:Latitude;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-2:2;\n"
        "B-" << iBIndex << "\\MWN-1-1-2-1:21;\n"
        "B-" << iBIndex << "\\MBM-1-1-2-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-2-1:MSB;\n"
        "B-" << iBIndex << "\\MWN-1-1-2-2:22;\n"
        "B-" << iBIndex << "\\MBM-1-1-2-2:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-2-2:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-3:Longitude;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-3:2;\n"
        "B-" << iBIndex << "\\MWN-1-1-3-1:23;\n"
        "B-" << iBIndex << "\\MBM-1-1-3-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-3-1:MSB;\n"
        "B-" << iBIndex << "\\MWN-1-1-3-2:24;\n"
        "B-" << iBIndex << "\\MBM-1-1-3-2:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-3-2:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-4:Altitude;\n"
        "B-" << iBIndex << "\\MT-1-1-4:D;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-4:1;\n"
        "B-" << iBIndex << "\\MWN-1-1-4-1:25;\n"
        "B-" << iBIndex << "\\MBM-1-1-4-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-4-1:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-5:Pitch;\n"
        "B-" << iBIndex << "\\MT-1-1-5:D;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-5:1;\n"
        "B-" << iBIndex << "\\MWN-1-1-5-1:11;\n"
        "B-" << iBIndex << "\\MBM-1-1-5-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-5-1:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-6:Roll;\n"
        "B-" << iBIndex << "\\MT-1-1-6:D;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-6:1;\n"
        "B-" << iBIndex << "\\MWN-1-1-6-1:10;\n"
        "B-" << iBIndex << "\\MBM-1-1-6-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-6-1:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-7:True Heading;\n"
        "B-" << iBIndex << "\\MT-1-1-7:D;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-7:1;\n"
        "B-" << iBIndex << "\\MWN-1-1-7-1:12;\n"
        "B-" << iBIndex << "\\MBM-1-1-7-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-7-1:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-8:Velocity X;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-8:2;\n"
        "B-" << iBIndex << "\\MWN-1-1-8-1:3;\n"
        "B-" << iBIndex << "\\MBM-1-1-8-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-8-1:MSB;\n"
        "B-" << iBIndex << "\\MWN-1-1-8-2:4;\n"
        "B-" << iBIndex << "\\MBM-1-1-8-2:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-8-2:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-9:Velocity Y;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-9:2;\n"
        "B-" << iBIndex << "\\MWN-1-1-9-1:5;\n"
        "B-" << iBIndex << "\\MBM-1-1-9-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-9-1:MSB;\n"
        "B-" << iBIndex << "\\MWN-1-1-9-2:6;\n"
        "B-" << iBIndex << "\\MBM-1-1-9-2:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-9-2:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-10:Velocity Z;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-10:2;\n"
        "B-" << iBIndex << "\\MWN-1-1-10-1:7;\n"
        "B-" << iBIndex << "\\MBM-1-1-10-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-10-1:MSB;\n"
        "B-" << iBIndex << "\\MWN-1-1-10-2:8;\n"
        "B-" << iBIndex << "\\MBM-1-1-10-2:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-10-2:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-11:Accel X;\n"
        "B-" << iBIndex << "\\MT-1-1-11:D;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-11:1;\n"
        "B-" << iBIndex << "\\MWN-1-1-11-1:14;\n"
        "B-" << iBIndex << "\\MBM-1-1-11-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-11-1:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-12:Accel Y;\n"
        "B-" << iBIndex << "\\MT-1-1-12:D;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-12:1;\n"
        "B-" << iBIndex << "\\MWN-1-1-12-1:15;\n"
        "B-" << iBIndex << "\\MBM-1-1-12-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-12-1:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-13:Accel Z;\n"
        "B-" << iBIndex << "\\MT-1-1-13:D;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-13:1;\n"
        "B-" << iBIndex << "\\MWN-1-1-13-1:16;\n"
        "B-" << iBIndex << "\\MBM-1-1-13-1:FW;\n"
        "B-" << iBIndex << "\\MTO-1-1-13-1:MSB;\n"
        "B-" << iBIndex << "\\MN-1-1-14:Ground Speed;\n"
        "B-" << iBIndex << "\\NML\\N-1-1-14:1;\n";
    iBIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Nav Status;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:INT;\n"
        "C-" << iCIndex << "\\BWT\\N:16;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:NON;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Latitude;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\BWT\\N:32;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:8.38190317E-8;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Longitude;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\BWT\\N:32;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:8.38190317E-8;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Altitude;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\BWT\\N:16;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:4;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Pitch;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\BWT\\N:16;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:5.49316406E-3;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Roll;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\BWT\\N:16;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:5.49316406E-3;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:True Heading;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:UNS;\n"
        "C-" << iCIndex << "\\BWT\\N:16;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:5.49316406E-3;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Velocity X;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\BWT\\N:32;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:3.814697265625E-06;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Velocity Y;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\BWT\\N:32;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:3.814697265625E-06;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Velocity Z;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\BWT\\N:32;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:3.814697265625E-06;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Accel X;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:0.03125;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Accel Y;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:0.03125;\n";
    iCIndex++;

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Accel Z;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:TWO;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:COE;\n"
        "C-" << iCIndex << "\\CO\\N:1;\n"
        "C-" << iCIndex << "\\CO1:N;\n"
        "C-" << iCIndex << "\\CO-1:0.03125;\n";
    iCIndex++;  

    ssTMATS <<
        "C-" << iCIndex << "\\DCN:Ground Speed;\n"
        "C-" << iCIndex << "\\MN1:Calculated Ground Speed;\n"
        "C-" << iCIndex << "\\MN3:knots;\n"
        "C-" << iCIndex << "\\MN4:OTH;\n"
        "C-" << iCIndex << "\\BFM:FPT;\n"
        "C-" << iCIndex << "\\MC\\N:0;\n"
        "C-" << iCIndex << "\\MA\\N:0;\n"
        "C-" << iCIndex << "\\SR:0;\n"
        "C-" << iCIndex << "\\DCT:DER;\n"
        "C-" << iCIndex << "\\DPA:Sqrt(a*a+b*b)*3600/6076;\n"
        "C-" << iCIndex << "\\DPNO:1;\n"
        "C-" << iCIndex << "\\DP\\N:2;\n"
        "C-" << iCIndex << "\\DP-1:Velocity X;\n"
        "C-" << iCIndex << "\\DP-2:Velocity Y;\n"
        "C-" << iCIndex << "\\DPC\\N:0;\n";
    iCIndex++;

    return ssTMATS.str();
    } // end TMATS()
