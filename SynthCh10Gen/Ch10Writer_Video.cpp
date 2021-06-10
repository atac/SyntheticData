
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

#include "Ch10Writer.h"
#include "Ch10Writer_Video.h"

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClCh10Writer_VideoF0::ClCh10Writer_VideoF0()
    {
    }


ClCh10Writer_VideoF0::~ClCh10Writer_VideoF0()
    {
    }

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void ClCh10Writer_VideoF0::Init(int iHandle, unsigned int uChanID)
    {
    this->iHandle = iHandle;
    this->uChanID = uChanID;

    // Setup the Ch 10 header
    iHeaderInit(&suCh10Header, uChanID, I106CH10_DTYPE_VIDEO_FMT_0, I106CH10_PFLAGS_CHKSUM_NONE | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
    suCh10Header.ubyHdrVer = CH10_VER_HDR_VIDEO;

    // Init CSDW
    memset(&suVideoF0CSDW, 0, 4);
    suVideoF0CSDW.bET   = 0;
    suVideoF0CSDW.bIPH  = 0;
    suVideoF0CSDW.bSRS  = 0;
    suVideoF0CSDW.bKLV  = 0;
    suVideoF0CSDW.uType = 0;    // MPEG-2 MP@ML guess!
    suVideoF0CSDW.uBA   = 0;    // Little-endian
    }


// ----------------------------------------------------------------------------

// Return a string with the TMATS R section for this channel

std::string ClCh10Writer_VideoF0::TMATS(int iRSection, int iEnumN, std::string sDescription)
    {
    std::stringstream   ssTMATS;
    std::string         sDSI;

    if (sDescription != "")
        sDSI = sDescription;
    else
        sDSI = "VideoInChan" + uChanID;

    ssTMATS <<
        "R-" << iRSection << "\\TK1-"  << iEnumN << ":" << uChanID << ";\n"
        "R-" << iRSection << "\\TK4-"  << iEnumN << ":" << uChanID << ";\n"
        "R-" << iRSection << "\\CHE-"  << iEnumN << ":T;\n"
        "R-" << iRSection << "\\DSI-"  << iEnumN << ":" << sDSI << ";\n"
        "R-" << iRSection << "\\CDT-"  << iEnumN << ":VIDIN;\n"
        "R-" << iRSection << "\\CDLN-" << iEnumN << ":VideoInChan" << uChanID << ";\n"
        "R-" << iRSection << "\\VTF-"  << iEnumN << ":0;\n"      // MPEG-2/H.264
        "R-" << iRSection << "\\VST-"  << iEnumN << ":5;\n"      // RGB
        "R-" << iRSection << "\\VED-"  << iEnumN << ":300;\n";   // Video encoding delay

#if 0
        "M-1\\ID:VideoInChan1;\n"
        "M-1\\BSG1:PCM;\n"
        "M-1\\BB\\DLN:VideoInChan1;\n"
        "P-1\\DLN:VideoInChan1;\n"
        "P-1\\D2:4000000;\n"
#endif

    return ssTMATS.str();
    } // end TMATS()


// ----------------------------------------------------------------------------

void ClCh10Writer_VideoF0::Write(int64_t * pullRelTime, uint8_t * pDataBuff, int iDataLen)
    {
    EnI106Status    enStatus;
    int             iFillerLen;
    unsigned char   achFiller[8];
    int             iBytesLeftToWrite;
    int             iBytesToWrite;

    for (int iSwapIdx=0; iSwapIdx<iDataLen; iSwapIdx+=2)
        std::swap(pDataBuff[iSwapIdx], pDataBuff[iSwapIdx+1]);

    // Max IRIG packet size is 524,288 so we may need to do several writes
    iBytesLeftToWrite = iDataLen;
    while (iBytesLeftToWrite > 0)
        {
        // Max write size is 2788 188-byte TS packets, do a little less to be safe.
        iBytesToWrite = MIN(iBytesLeftToWrite, 2500*188);

        // Update the packet header, 
        suCh10Header.ulDataLen   = sizeof(suVideoF0CSDW) + iBytesToWrite;
        vLLInt2TimeArray(pullRelTime, suCh10Header.aubyRefTime);
        iFillerLen = 8;
        uAddDataFillerChecksum2(&suCh10Header, &suVideoF0CSDW, sizeof(suVideoF0CSDW), pDataBuff, achFiller, &iFillerLen);
        suCh10Header.uChecksum = uCalcHeaderChecksum(&suCh10Header);

        enStatus = enI106Ch10WriteMsg2(iHandle, &suCh10Header, &suVideoF0CSDW, sizeof(suVideoF0CSDW), pDataBuff, achFiller, iFillerLen);

        suCh10Header.ubySeqNum++;
        iBytesLeftToWrite -= iBytesToWrite;
        } // end while there are bytes to write

#if 0
    uint32_t    uDataBuffLen;

    // Make sure the data buffer is big enough to hold the filler and checksum
    int     iChecksumType = suWriteMsg1553.suCh10Header.ubyPacketFlags & I106CH10_PFLAGS_CHKSUM_MASK;
    uDataBuffLen = uCalcDataBuffReqSize(suWriteMsg1553.suCh10Header.ulDataLen, iChecksumType);

    if (uDataBuffLen > suWriteMsg1553.uBuffLen)
        {
        suWriteMsg1553.uBuffLen += 1000;
        suWriteMsg1553.pchDataBuff = (unsigned char *)realloc(suWriteMsg1553.pchDataBuff, suWriteMsg1553.uBuffLen);
        suWriteMsg1553.psu1553CSDW = (Su1553F1_ChanSpec *)suWriteMsg1553.pchDataBuff;
        }

    // Put a checksum on the end of the packet
    uAddDataFillerChecksum(&(suWriteMsg1553.suCh10Header), suWriteMsg1553.pchDataBuff);

    // Update the packet length and data length fields
    suWriteMsg1553.suCh10Header.ulPacketLen = iGetHeaderLen(&(suWriteMsg1553.suCh10Header)) + uDataBuffLen;
//    suWriteMsg1553.suCh10Header.ulDataLen = suWriteMsg1553.uDataLen;
    suWriteMsg1553.suCh10Header.uChecksum = uCalcHeaderChecksum(&(suWriteMsg1553.suCh10Header));

    // Write it
    enI106Ch10WriteMsg(iHandle, &(suWriteMsg1553.suCh10Header), suWriteMsg1553.pchDataBuff);

    // Reset the buffer
    suWriteMsg1553.suCh10Header.ubySeqNum++;
    suWriteMsg1553.suCh10Header.ulDataLen = 4;
    suWriteMsg1553.psu1553CSDW->uMsgCnt = 0;
#endif

    return;
    } // end Write()
