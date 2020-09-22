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

    psuInsData->uStatus      = 0x0020;
    //psuInsData->uTimeTag
    lTempVel = ( int32_t)(pclSimState->fState["BM.acvxi"] * 0x00040000);
    psuInsData->sVelX_MSW    = ( int16_t)((lTempVel >> 16) & 0x0000ffff);
    psuInsData->uVelX_LSW    = (uint16_t)((lTempVel      ) & 0x0000ffff);
    lTempVel = (int32_t)(pclSimState->fState["BM.acvyi"] * 0x00040000);
    psuInsData->sVelY_MSW    = ( int16_t)((lTempVel >> 16) & 0x0000ffff);
    psuInsData->uVelY_LSW    = (uint16_t)((lTempVel      ) & 0x0000ffff);
    lTempVel = (int32_t)(pclSimState->fState["BM.acvzi"] * 0x00040000);
    psuInsData->sVelZ_MSW    = ( int16_t)((lTempVel >> 16) & 0x0000ffff);
    psuInsData->uVelZ_LSW    = (uint16_t)((lTempVel      ) & 0x0000ffff);
    psuInsData->uAz          = (uint16_t)(pclSimState->fState["BM.acpsid"]  / 180.0 * (double)0x7fff);
    psuInsData->sRoll        = ( int16_t)(pclSimState->fState["BM.acphid"]  / 180.0 * (double)0x7fff);
    psuInsData->sPitch       = ( int16_t)(pclSimState->fState["BM.acthtad"] / 180.0 * (double)0x7fff);
    psuInsData->uTrueHeading = (uint16_t)(pclSimState->fState["BM.acpsid"]  / 180.0 * (double)0x7fff);
    psuInsData->uMagHeading  = (uint16_t)(pclSimState->fState["BM.acmagd"]  / 180.0 * (double)0x7fff);
    psuInsData->sAccX        = (int16_t)pclSimState->fState["BM.acaxi"];
    psuInsData->sAccY        = (int16_t)pclSimState->fState["BM.acayi"];
    psuInsData->sAccZ        = (int16_t)pclSimState->fState["BM.acazi"];
    //psuInsData->sCXX_MSW
    //psuInsData->uCXX_LSW
    //psuInsData->sCXY_MSW
    //psuInsData->uCXY_LSW
    ulTempLatLon = (uint32_t)(pclSimState->fState["BM.aclatd"] / 180.0 * (double)0x7fffffff);
    psuInsData->sCXZ_MSW     = ((ulTempLatLon >> 16) & 0x0000ffff);
    psuInsData->uCXZ_LSW     = ( ulTempLatLon        & 0x0000ffff);
    ulTempLatLon = (uint32_t)(pclSimState->fState["BM.aclond"] / 180.0 * (double)0x7fffffff);
    psuInsData->sLon_MSW     = ((ulTempLatLon >> 16) & 0x0000ffff);
    psuInsData->uLon_LSW     = ( ulTempLatLon        & 0x0000ffff);
    psuInsData->sAlt         = (int16_t)(pclSimState->fState["BM.acaltf"] / 4.0);
    //psuInsData->sSteeringError
    //psuInsData->sTiltX
    //psuInsData->sTiltY
    //psuInsData->sJustInCase[4]
    }


