#pragma once

#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"

#include "i106_time.h"
#include "i106_index.h"
#include "i106_decode_time.h"
#include "i106_decode_1553f1.h"
#include "i106_decode_tmats.h"
#include "i106_decode_index.h"

#include "Common.h"
#include "SimState.h"

#include "Ch10Writer.h"
#include "Ch10Channel.h";

#include <vector>;
#include <assert.h>;

using namespace Irig106;

class TmatsFormatter
{
public:

  static void WriteTMATS(
    int fileHandle, 
    std::string programName, 
    double currSimClockTime, 
    std::vector<Ch10Channel*>* channels) 
  {

    SuI106Ch10Header    suI106Hdr;
    std::stringstream   ssTMATS;
    uint8_t*            pchDataBuff;
    uint32_t            ulDataBuffSize;
    SuTmats_ChanSpec*   psuTmats_ChanSpec;
    ClTmatsIndexes      tmatsIndex;
    
    const int dataChannelCount = channels->size();

    // Make current time string
    time_t        iCurrTime;
    char          szCurrTime[100];
    struct tm* psuCurrTime;
    iCurrTime = time(NULL);
    psuCurrTime = gmtime(&iCurrTime);
    strftime(szCurrTime, sizeof(szCurrTime), "%m-%d-%Y-%H-%M-%S", psuCurrTime);

    // Make a time string for the TMATS in the format 08-19-2014-17-33-59
    time_t        iCurrSimClockTime;
    char          szCurrSimClockTime[100];
    struct tm* psuCurrSimClockTime;
    iCurrSimClockTime = (time_t)currSimClockTime;
    psuCurrSimClockTime = gmtime(&iCurrSimClockTime);
    strftime(szCurrSimClockTime, sizeof(szCurrSimClockTime), "%m-%d-%Y-%H-%M-%S", psuCurrSimClockTime);

    ssTMATS.clear();
    ssTMATS <<
      "COMMENT:**********************************************************************;\n"
      "COMMENT: Synthetic data file created with SynthCh10Gen on " << szCurrTime << ";\n"
      "COMMENT: See https://github.com/atac/SyntheticData for details;\n"
      "COMMENT:**********************************************************************;\n"
      "COMMENT:                           G Group                                    ;\n"
      "COMMENT:**********************************************************************;\n";
    ssTMATS <<
      "G\\PN:" << programName << ";\n";
    ssTMATS <<
      "G\\106:" CH10_VER_TMATS ";\n"
      "G\\DSI\\N:1;\n"
      "G\\SC:UNCLASSIFIED;\n"
      "G\\DSI-1:DATASOURCE;\n"
      "G\\DST-1:STO;\n"
      "G\\DSC-1:UNCLASSIFIED;\n";

    ssTMATS <<
      "COMMENT:**********************************************************************;\n"
      "COMMENT:                           R Group                                    ;\n"
      "COMMENT:**********************************************************************;\n"
      "R-1\\ID:DATASOURCE;\n"
      "R-1\\RID:SynthCh10Gen;\n"
      "R-1\\NSB:0;\n"
      "R-1\\RI1:Avionics Test and Analysis Corp (www.avtest.com);\n"
      "R-1\\RI2:SynthCh10Gen;\n"
      "R-1\\RI3:Y;\n";
    ssTMATS <<
      "R-1\\RI4:" << szCurrSimClockTime << ";\n";
    ssTMATS <<
      "R-1\\RI6:N;\n"
      "R-1\\CRE:Y;\n"
      "R-1\\RSS:C;\n"
      "R-1\\RML:I;\n"
      "R-1\\ERBS:AUTO;\n"
      "R-1\\EV\\E:F;\n"
      "R-1\\IDX\\E:T;\n"
      "R-1\\N:" << dataChannelCount << ";\n";

    for (auto c : *channels) {
      ssTMATS << c->GetTMATS(tmatsIndex);
      UpdateTmatsIndexes(tmatsIndex, c->Type());
    }

    assert(tmatsIndex.iRSrcNum - 1 == dataChannelCount);

    // Form the TMATS packet header
    iHeaderInit(&suI106Hdr, 0, I106CH10_DTYPE_TMATS, I106CH10_PFLAGS_CHKSUM_NONE | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
    suI106Hdr.ulDataLen = sizeof(SuTmats_ChanSpec) + ssTMATS.str().length();
    suI106Hdr.ubyHdrVer = CH10_VER_HDR_TMATS;
    SimClockToRel(fileHandle, currSimClockTime, suI106Hdr.aubyRefTime);
    //    memset(suI106Hdr.aubyRefTime, 0, 6);

        // Setup the TMATS packet data portion
    ulDataBuffSize = uCalcDataBuffReqSize(suI106Hdr.ulDataLen, I106CH10_PFLAGS_CHKSUM_NONE);
    pchDataBuff = (uint8_t*)malloc(ulDataBuffSize);
    memset(pchDataBuff, 0, ulDataBuffSize);
    psuTmats_ChanSpec = (SuTmats_ChanSpec*)pchDataBuff;
    psuTmats_ChanSpec->iCh10Ver = CH10_VER_RECORDER;
    memcpy(&pchDataBuff[4], ssTMATS.str().c_str(), ssTMATS.str().length());
    uAddDataFillerChecksum(&suI106Hdr, pchDataBuff);
    suI106Hdr.uChecksum = uCalcHeaderChecksum(&suI106Hdr);

    enI106Ch10WriteMsg(fileHandle, &suI106Hdr, pchDataBuff);
    free(pchDataBuff);
  }

  static void UpdateTmatsIndexes(ClTmatsIndexes& index, Ch10Channel::ChannelType type) {
    switch (type) {
    case Ch10Channel::ChannelType::A429:
    case Ch10Channel::ChannelType::MS1553:
      index.iBIndex++;
      index.iRIndex++;
      break;
    case Ch10Channel::ChannelType::PCM:
    case Ch10Channel::ChannelType::Video:
    case Ch10Channel::ChannelType::Time:
      index.iRIndex++;
      break;
    default:
      break;
    }
  }

  static void SimClockToRel(int iI106Handle, double dSimTime, uint8_t abyRelTime[])
  {
    SuIrig106Time   suIrigTime;

    suIrigTime.enFmt = I106_DATEFMT_DMY;
    suIrigTime.ulSecs = (unsigned long)dSimTime;
    suIrigTime.ulFrac = (unsigned long)((dSimTime - (unsigned long)dSimTime) * 9999999.9);
    enI106_Irig2RelTime(iI106Handle, &suIrigTime, abyRelTime);

    return;
  }

};

