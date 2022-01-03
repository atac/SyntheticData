

//#include <cstdio>
//#include <cassert>
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
#include "i106_decode_arinc429.h"

using namespace Irig106;

#include "Ch10Format_A429.h"


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

// Constructor
ClCh10Format_ARINC429::ClCh10Format_ARINC429()
    {
//    iSubchannels = 0;
    }

// ----------------------------------------------------------------------------

// Destructor
ClCh10Format_ARINC429::~ClCh10Format_ARINC429()
    {

    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

// Set the relative time counter

void ClCh10Format_ARINC429::SetRTC(int64_t * pllRefTime)
    {
    llRefTime = *pllRefTime;
    }


// ----------------------------------------------------------------------------

// Calculate and return the correct value of the ARINC 429 parity bit

int ClCh10Format_ARINC429::CalcParity(SuArinc429F0_Data * psuA429Data)
    {
    int         iParityCalc;
    int         iBitIdx;
    int32_t     iData;

    iData = *((uint32_t *)psuA429Data);
    
    // ARINC 429 is odd parity so start with 1
    iParityCalc = 1;

    // Count all the 1's in the lower 31 bits of the data word
    for (iBitIdx = 1; iBitIdx <= 31; iBitIdx++)
        {
        if ((iData & 0x0001) == 1)
            iParityCalc++;
        iData = iData >> 1;
        }

    return (iParityCalc & 0x0001);
    }


// ----------------------------------------------------------------------------

unsigned char ClCh10Format_ARINC429::ReverseLabel(unsigned char uLabel)
    {
    unsigned char   uRLabel;
    int             iBitIdx;

    uRLabel = 0;
    for (iBitIdx=0; iBitIdx<8; iBitIdx++)
        {
        uRLabel <<= 1;
        uRLabel  |= uLabel & 0x01;
        uLabel  >>= 1;
        }

    return uRLabel;
    }



