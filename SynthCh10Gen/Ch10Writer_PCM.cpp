

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_tmats.h"
#include "i106_decode_tmats_r.h"
#include "i106_decode_pcmf1.h"

#include "Ch10Writer_PCM.h"


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClCh10Writer_PCM::ClCh10Writer_PCM()
    {
    }


ClCh10Writer_PCM::~ClCh10Writer_PCM()
    {
    }

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void ClCh10Writer_PCM::Init(int iHandle, unsigned int uChanID)
    {
    this->iHandle = iHandle;
    this->uChanID = uChanID;

    // Setup the Ch 10 header
    iHeaderInit(&(suWriteMsgPCM.suCh10Header), uChanID, I106CH10_DTYPE_PCM_FMT_1, I106CH10_PFLAGS_CHKSUM_32 | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
    suWriteMsgPCM.suCh10Header.ubyHdrVer = 3;
    suWriteMsgPCM.suCh10Header.ulDataLen = 4;

    // Setup a buffer with enough memory to handle CSDW for now
    suWriteMsgPCM.uBuffLen = 4;
    suWriteMsgPCM.suCh10Header.ulDataLen = 4;
    suWriteMsgPCM.pchDataBuff = (unsigned char *)malloc(suWriteMsgPCM.uBuffLen);
    suWriteMsgPCM.psuPCM_CSDW = (SuPcmF1_ChanSpec *)suWriteMsgPCM.pchDataBuff;

    // Init CSDW
    memset(suWriteMsgPCM.psuPCM_CSDW, 0, 4);
//    suWriteMsgPCM.psuPCM_CSDW->

    // TMATS Channel Data Link Name for this channel
    std::stringstream   ssCDLN;
    ssCDLN << "PCMInChan" << uChanID;
    sCDLN = ssCDLN.str();

    }


// ----------------------------------------------------------------------------

// Return a string with the TMATS R section for this channel

std::string ClCh10Writer_PCM::TMATS(int iRSection, int iEnumN)
    {
    std::stringstream   ssTMATS;

    ssTMATS <<
        "R-" << iRSection << "\\TK1-"  << iEnumN << ":" << uChanID << ";\n"
        "R-" << iRSection << "\\TK4-"  << iEnumN << ":" << uChanID << ";\n"
        "R-" << iRSection << "\\CHE-"  << iEnumN << ":T;\n"
        "R-" << iRSection << "\\DSI-"  << iEnumN << ":PCMInChan" << uChanID << ";\n"
        "R-" << iRSection << "\\CDT-"  << iEnumN << ":PCMIN;\n"
        "R-" << iRSection << "\\CDLN-" << iEnumN << ":" << sCDLN << ";\n";

    return ssTMATS.str();
    } // end TMATS()

// ----------------------------------------------------------------------------

// Append a PCM frame/subframe to the end of a PCM packet.

void ClCh10Writer_PCM::AppendMsg(SuPcmF1_IntraPktHeader * psuPCM_IPH, uint16_t auData[], unsigned ulDataLen)
    {
    unsigned        uCurrBufferOffset;

    // If there is no data yet then the packet RTC is the first message RTC
    if (suWriteMsgPCM.suCh10Header.ulDataLen <= 4)
        {
        // This assumes intra-packet time is in RTC format
        memcpy(suWriteMsgPCM.suCh10Header.aubyRefTime, psuPCM_IPH->aubyIntPktTime, 6);
        }

    uCurrBufferOffset = suWriteMsgPCM.suCh10Header.ulDataLen;

    // Expand the PCM packet buffer if necessary
    if (suWriteMsgPCM.psuPCM_CSDW->bIntraPktHdr == 1)
        suWriteMsgPCM.suCh10Header.ulDataLen += sizeof(SuPcmF1_IntraPktHeader);
    suWriteMsgPCM.suCh10Header.ulDataLen += ulDataLen;

    if (suWriteMsgPCM.suCh10Header.ulDataLen > suWriteMsgPCM.uBuffLen)
        {
        suWriteMsgPCM.uBuffLen += 1000;
        suWriteMsgPCM.pchDataBuff = (unsigned char *)realloc(suWriteMsgPCM.pchDataBuff, suWriteMsgPCM.uBuffLen);
        suWriteMsgPCM.psuPCM_CSDW = (SuPcmF1_ChanSpec *)suWriteMsgPCM.pchDataBuff;
        }
    
    // Build one of the various packet layouts

    // Intrapacket header
    if (suWriteMsgPCM.psuPCM_CSDW->bIntraPktHdr == 1)
        {
        memcpy(suWriteMsgPCM.pchDataBuff + uCurrBufferOffset, psuPCM_IPH, sizeof(SuPcmF1_IntraPktHeader));
        uCurrBufferOffset += sizeof(SuPcmF1_IntraPktHeader);
        }

    // Data
    memcpy(suWriteMsgPCM.pchDataBuff + uCurrBufferOffset, auData, ulDataLen);
    uCurrBufferOffset += ulDataLen;

    } // end AppendMsg()


// ----------------------------------------------------------------------------

void ClCh10Writer_PCM::Commit()
    {
    uint32_t    uDataBuffLen;

    // Make sure the data buffer is big enough to hold the filler and checksum
    int     iChecksumType = suWriteMsgPCM.suCh10Header.ubyPacketFlags & I106CH10_PFLAGS_CHKSUM_MASK;
    uDataBuffLen = uCalcDataBuffReqSize(suWriteMsgPCM.suCh10Header.ulDataLen, iChecksumType);

    if (uDataBuffLen > suWriteMsgPCM.uBuffLen)
        {
        suWriteMsgPCM.uBuffLen += 1000;
        suWriteMsgPCM.pchDataBuff = (unsigned char *)realloc(suWriteMsgPCM.pchDataBuff, suWriteMsgPCM.uBuffLen);
        suWriteMsgPCM.psuPCM_CSDW = (SuPcmF1_ChanSpec *)suWriteMsgPCM.pchDataBuff;
        }

    // Put a checksum on the end of the packet
    uAddDataFillerChecksum(&(suWriteMsgPCM.suCh10Header), suWriteMsgPCM.pchDataBuff);

    // Update the packet length and data length fields
    suWriteMsgPCM.suCh10Header.ulPacketLen = iGetHeaderLen(&(suWriteMsgPCM.suCh10Header)) + uDataBuffLen;
    suWriteMsgPCM.suCh10Header.uChecksum = uCalcHeaderChecksum(&(suWriteMsgPCM.suCh10Header));

    // Write it
    enI106Ch10WriteMsg(iHandle, &(suWriteMsgPCM.suCh10Header), suWriteMsgPCM.pchDataBuff);

    // Reset the buffer
    suWriteMsgPCM.suCh10Header.ubySeqNum++;
    suWriteMsgPCM.suCh10Header.ulDataLen = 4;

    return;
    } // Commit()
