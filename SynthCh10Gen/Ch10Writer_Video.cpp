
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <assert.h>

#include <string>
#include <iostream>     // std::cout
#include <sstream>      // std::stringstream

#include "irig106ch10.h"

#include "Common.h"
#include "Ch10Writer_Video.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

Ch10Writer_Video::Ch10Writer_Video()
{
  this->formatter = nullptr;
}


Ch10Writer_Video::~Ch10Writer_Video()
{
}

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void Ch10Writer_Video::Init(int iHandle, unsigned int uChanID, Ch10Formatter_Video* formatter)
{
  Ch10Writer::Init(iHandle, uChanID);

  this->formatter = formatter;

  // Setup the Ch 10 header
  iHeaderInit(&suCh10Header, uChanID, I106CH10_DTYPE_VIDEO_FMT_0, I106CH10_PFLAGS_CHKSUM_NONE | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
  suCh10Header.ubyHdrVer = CH10_VER_HDR_VIDEO;

  // Init data buffer
  bufLen = 1000;
  dataBuf = (uint8_t*)malloc(bufLen);
  currBufOffset = sizeof(SuVideoF0_ChanSpec);

  // Init CSDW
  suVideoF0CSDW = (SuVideoF0_ChanSpec*)dataBuf;
  memset(suVideoF0CSDW, 0, 4);
  suVideoF0CSDW->bET = 0;
  suVideoF0CSDW->bIPH = 0;
  suVideoF0CSDW->bSRS = 0;
  suVideoF0CSDW->bKLV = 0;
  suVideoF0CSDW->uType = 0;    // MPEG-2 MP@ML guess!
  suVideoF0CSDW->uBA = 0;    // Little-endian
}


// ----------------------------------------------------------------------------

// Return a string with the TMATS R section for this channel

std::string Ch10Writer_Video::TMATS(ClTmatsIndexes& TmatsIndex, std::string sCDLN)

{
  std::stringstream   ssTMATS;

  ssTMATS <<
    "R-" << TmatsIndex.iRIndex << "\\DSI-" << TmatsIndex.iRSrcNum << ":" << sCDLN << ";\n"
    "R-" << TmatsIndex.iRIndex << "\\TK1-" << TmatsIndex.iRSrcNum << ":" << uChanID << ";\n"
    "R-" << TmatsIndex.iRIndex << "\\TK4-" << TmatsIndex.iRSrcNum << ":" << uChanID << ";\n"
    "R-" << TmatsIndex.iRIndex << "\\CHE-" << TmatsIndex.iRSrcNum << ":T;\n"
    "R-" << TmatsIndex.iRIndex << "\\CDT-" << TmatsIndex.iRSrcNum << ":VIDIN;\n"
    "R-" << TmatsIndex.iRIndex << "\\CDLN-" << TmatsIndex.iRSrcNum << ":" << sCDLN << ";\n"
    "R-" << TmatsIndex.iRIndex << "\\VTF-" << TmatsIndex.iRSrcNum << ":0;\n"      // MPEG-2/H.264
    "R-" << TmatsIndex.iRIndex << "\\VST-" << TmatsIndex.iRSrcNum << ":5;\n"      // RGB
    "R-" << TmatsIndex.iRIndex << "\\VED-" << TmatsIndex.iRSrcNum << ":300;\n";   // Video encoding delay

  ssTMATS << formatter->TMATS(TmatsIndex, sCDLN);

  return ssTMATS.str();
} // end TMATS()


void Ch10Writer_Video::AppendMsg()
{
  // F0 IPH is optional

  if (formatter->videoData == nullptr)
    return; // nothing to do

  // If there is no data yet then the packet RTC is the first message RTC
  if (currBufOffset <= 4)
    vLLInt2TimeArray(formatter->GetRTC(), suCh10Header.aubyRefTime);

  unsigned dataLen = currBufOffset + formatter->videoData->size();

  if (dataLen > bufLen)
  {
    bufLen = dataLen + 1000;
    dataBuf = (uint8_t*)realloc(dataBuf, bufLen);
    suVideoF0CSDW = (SuVideoF0_ChanSpec*)dataBuf;
  }

  memcpy(dataBuf + currBufOffset, formatter->videoData->data(), formatter->videoData->size());
  currBufOffset = dataLen;
}

void Ch10Writer_Video::Commit()
{
  EnI106Status    enStatus;
  int             iFillerLen;
  unsigned char   achFiller[8];
  int             iBytesLeftToWrite;
  int             iBytesToWrite;

  // byteswap video data
  for (int iSwapIdx = 0; iSwapIdx < bufLen; iSwapIdx += 2)
    std::swap(dataBuf[iSwapIdx], dataBuf[iSwapIdx + 1]);

  // Max IRIG packet size is 524,288 so we may need to do several writes
  iBytesLeftToWrite = currBufOffset;
  while (iBytesLeftToWrite > 0)
  {
    // Max write size is 2788 188-byte TS packets, do a little less to be safe.
    iBytesToWrite = MIN(iBytesLeftToWrite, 2500 * 188);

    // TODO: check dataBuf pointers to see if this would even work in the case where multiple loop iterations happen

    // Update the packet header, 
    suCh10Header.ulDataLen = sizeof(suVideoF0CSDW) + iBytesToWrite;
    iFillerLen = 8;
    uAddDataFillerChecksum2(&suCh10Header, &suVideoF0CSDW, sizeof(suVideoF0CSDW), dataBuf, iBytesToWrite, achFiller, &iFillerLen);
    suCh10Header.uChecksum = uCalcHeaderChecksum(&suCh10Header);

    enStatus = enI106Ch10WriteMsg2(iHandle, &suCh10Header, &suVideoF0CSDW, sizeof(suVideoF0CSDW), dataBuf, iBytesToWrite, achFiller, iFillerLen);

    suCh10Header.ubySeqNum++;
    iBytesLeftToWrite -= iBytesToWrite;
  } // end while there are bytes to write
   
  // Reset buffer
  currBufOffset = sizeof(SuVideoF0_ChanSpec);
}













//// ----------------------------------------------------------------------------
//
//void Ch10Writer_Video::Write(int64_t* pullRelTime, uint8_t* pDataBuff, int iDataLen)
//{
//
////#if 0
////  uint32_t    uDataBuffLen;
////
////  // Make sure the data buffer is big enough to hold the filler and checksum
////  int     iChecksumType = suWriteMsg1553.suCh10Header.ubyPacketFlags & I106CH10_PFLAGS_CHKSUM_MASK;
////  uDataBuffLen = uCalcDataBuffReqSize(suWriteMsg1553.suCh10Header.ulDataLen, iChecksumType);
////
////  if (uDataBuffLen > suWriteMsg1553.uBuffLen)
////  {
////    suWriteMsg1553.uBuffLen += 1000;
////    suWriteMsg1553.pchDataBuff = (unsigned char*)realloc(suWriteMsg1553.pchDataBuff, suWriteMsg1553.uBuffLen);
////    suWriteMsg1553.psu1553CSDW = (Su1553F1_ChanSpec*)suWriteMsg1553.pchDataBuff;
////  }
////
////  // Put a checksum on the end of the packet
////  uAddDataFillerChecksum(&(suWriteMsg1553.suCh10Header), suWriteMsg1553.pchDataBuff);
////
////  // Update the packet length and data length fields
////  suWriteMsg1553.suCh10Header.ulPacketLen = iGetHeaderLen(&(suWriteMsg1553.suCh10Header)) + uDataBuffLen;
////  //    suWriteMsg1553.suCh10Header.ulDataLen = suWriteMsg1553.uDataLen;
////  suWriteMsg1553.suCh10Header.uChecksum = uCalcHeaderChecksum(&(suWriteMsg1553.suCh10Header));
////
////  // Write it
////  enI106Ch10WriteMsg(iHandle, &(suWriteMsg1553.suCh10Header), suWriteMsg1553.pchDataBuff);
////
////  // Reset the buffer
////  suWriteMsg1553.suCh10Header.ubySeqNum++;
////  suWriteMsg1553.suCh10Header.ulDataLen = 4;
////  suWriteMsg1553.psu1553CSDW->uMsgCnt = 0;
////#endif
//
//  return;
//} // end Write()
