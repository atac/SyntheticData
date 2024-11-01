
#include <cstdio>
#include <cassert>
#include <ctime>
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream
#include <string.h>
#include <stdlib.h>
#include <math.h>

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_time.h"
#include "i106_decode_1553f1.h"
//#include "i106_decode_tmats.h"

#include "Common.h"
#include "Ch10Writer.h"
#include "Ch10Writer_Time.h"
#include "Ch10Format_Time.h"


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClCh10Writer_Time::ClCh10Writer_Time()
{
  formatter = nullptr;
}


ClCh10Writer_Time::~ClCh10Writer_Time()
{
}

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------
void ClCh10Writer_Time::Init(int iHandle, unsigned int uChanID, Ch10Format_Time* formatter)
{
  Ch10Writer::Init(iHandle, uChanID);

  this->formatter = formatter;

  // Setup the Ch 10 header
  iHeaderInit(&(suWritePktTimeF1.suCh10Header), uChanID, I106CH10_DTYPE_IRIG_TIME, I106CH10_PFLAGS_CHKSUM_32 | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
  suWritePktTimeF1.suCh10Header.ubyHdrVer = CH10_VER_HDR_TIME;
  suWritePktTimeF1.suCh10Header.ulDataLen = sizeof(SuTimeF1_ChanSpec) + sizeof(SuTime_MsgDmyFmt);
}

void ClCh10Writer_Time::AppendMsg() {
  if (formatter != nullptr)
    currentTime = formatter->GetCurrentTime();
}

void ClCh10Writer_Time::Commit()
{
  SuIrig106Time       suIrigTime;
  struct tm* psuCurrSimClockTime;
  int                 iValToConvert;

  // Zero out the data packet
  memset(&(suWritePktTimeF1.suTimePktBuffer), 0, sizeof(suWritePktTimeF1.suTimePktBuffer));

  // Setup the CSDW
  suWritePktTimeF1.suTimePktBuffer.suTimeF1CSDW.uTimeSrc = I106_TIMESRC_INTERNAL;
  suWritePktTimeF1.suTimePktBuffer.suTimeF1CSDW.uTimeFmt = I106_TIMEFMT_INT_RTC;
  suWritePktTimeF1.suTimePktBuffer.suTimeF1CSDW.uDateFmt = 1; // DMY

  // Start making the time packet
  time_t  iCurrSimClockTime = (time_t)currentTime;
  psuCurrSimClockTime = gmtime(&iCurrSimClockTime);

  // Year
  iValToConvert = psuCurrSimClockTime->tm_year + 1900;
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uYn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uTYn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uHYn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uOYn = uLowBcdDigit(&iValToConvert);

  // Month
  iValToConvert = psuCurrSimClockTime->tm_mon + 1;
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uOn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uTOn = uLowBcdDigit(&iValToConvert);

  // Day
  iValToConvert = psuCurrSimClockTime->tm_mday;
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uDn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uTDn = uLowBcdDigit(&iValToConvert);

  // Hour
  iValToConvert = psuCurrSimClockTime->tm_hour;
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uHn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uTHn = uLowBcdDigit(&iValToConvert);

  // Minute
  iValToConvert = psuCurrSimClockTime->tm_min;
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uMn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uTMn = uLowBcdDigit(&iValToConvert);

  // Second
  iValToConvert = psuCurrSimClockTime->tm_sec;
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uSn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uTSn = uLowBcdDigit(&iValToConvert);

  // Milliseconds
  iValToConvert = (int)((currentTime - (int)currentTime) * 99.9);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uTmn = uLowBcdDigit(&iValToConvert);
  suWritePktTimeF1.suTimePktBuffer.suTimeDataBuff.uHmn = uLowBcdDigit(&iValToConvert);

  // Make the packet header
//    SimClockToRel(iHandle, fCurrSimClockTime, suWritePktTimeF1.suCh10Header.aubyRefTime);
  suIrigTime.enFmt = I106_DATEFMT_DMY;
  suIrigTime.ulSecs = (unsigned long)currentTime;
  suIrigTime.ulFrac = (unsigned long)((currentTime - (unsigned long)currentTime) * 9999999.9);
  enI106_Irig2RelTime(iHandle, &suIrigTime, suWritePktTimeF1.suCh10Header.aubyRefTime);

  uAddDataFillerChecksum(&(suWritePktTimeF1.suCh10Header), (unsigned char*)&(suWritePktTimeF1.suTimePktBuffer));
  suWritePktTimeF1.suCh10Header.uChecksum = uCalcHeaderChecksum(&(suWritePktTimeF1.suCh10Header));

  enI106Ch10WriteMsg(iHandle, &(suWritePktTimeF1.suCh10Header), &(suWritePktTimeF1.suTimePktBuffer));
  suWritePktTimeF1.suCh10Header.ubySeqNum++;

  return;
}



void ClCh10Writer_Time::SetRelTime(int64_t lSimClockTicks, double fSimClockTime)
{
  SuIrig106Time   suIrigTime;
  uint8_t         abyRelTime[6];

  vLLInt2TimeArray(&lSimClockTicks, abyRelTime);
  suIrigTime.enFmt = I106_DATEFMT_DMY;
  suIrigTime.ulSecs = (unsigned long)fSimClockTime;
  suIrigTime.ulFrac = (unsigned long)((fSimClockTime - (unsigned long)fSimClockTime) * 9999999.9);
  enI106_SetRelTime(iHandle, &suIrigTime, abyRelTime);
}

std::string ClCh10Writer_Time::TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN) {
  return formatter->TMATS(tmatsIndex, sCDLN, uChanID);
}


// ----------------------------------------------------------------------------

// Return the low BCD from an integer and then divide by 10 shifting the BCD
// value to the right one digit.

unsigned uLowBcdDigit(int* piValToConvert)
{
  unsigned    uBcdDigit;

  uBcdDigit = *piValToConvert % 10;
  *piValToConvert = *piValToConvert / 10;

  return uBcdDigit;
}
