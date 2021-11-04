#pragma once

//#include <cstdio>
//#include <cassert>
//#include <ctime>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
//#include <string.h>
//#include <stdlib.h>
//#include <math.h>

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_1553f1.h"

#include "Common.h"
#include "SimState.h"
#include "Ch10Format_1553.h"

using namespace Irig106;


class ClCh10Format_1553_Nav :
    public ClCh10Format_1553
    {
public:
    ClCh10Format_1553_Nav(unsigned uRT, unsigned bTR, unsigned uSubAddr, unsigned uWC);
    ClCh10Format_1553_Nav(unsigned uRT_Send,unsigned uRT_Rcv, unsigned uSubAddr_Send, unsigned uSubAddr_Rcv, unsigned uWC);
    ~ClCh10Format_1553_Nav();

#pragma pack(push,1)
    // F-16/C-130/A-10 EGI
    typedef struct INS_DataS {
      uint16_t  uStatus;            //  1
      uint16_t  uTimeTag;           //  2
       int16_t  sVelX_MSW;          //  3
      uint16_t  uVelX_LSW;          //  4
       int16_t  sVelY_MSW;          //  5
      uint16_t  uVelY_LSW;          //  6
       int16_t  sVelZ_MSW;          //  7
      uint16_t  uVelZ_LSW;          //  8
      uint16_t  uAz;                //  9
       int16_t  sRoll;              // 10
       int16_t  sPitch;             // 11
      uint16_t  uTrueHeading;       // 12
      uint16_t  uMagHeading;        // 13
       int16_t  sAccX;              // 14
       int16_t  sAccY;              // 15
       int16_t  sAccZ;              // 16
       int16_t  sCXX_MSW;           // 17
      uint16_t  uCXX_LSW;           // 18
       int16_t  sCXY_MSW;           // 19
      uint16_t  uCXY_LSW;           // 20
       int16_t  sLat_MSW;           // 21
      uint16_t  uLat_LSW;           // 22
       int16_t  sLon_MSW;           // 23
      uint16_t  uLon_LSW;           // 24
       int16_t  sAlt;               // 25
       int16_t  sSteeringError;     // 26
       int16_t  sTiltX;             // 27
       int16_t  sTiltY;             // 28
       int16_t  sJustInCase[4];     // 29-32
      } SuINS_Data;
#pragma pack(pop)


    // Class variables
    SuINS_Data * psuInsData;

    // Methods

    void MakeMsg(ClSimState * pclSimState);
    std::string TMATS(ClTmatsIndexes & TmatsIndex);

    };

