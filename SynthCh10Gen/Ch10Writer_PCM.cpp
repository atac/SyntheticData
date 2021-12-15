

// irig106lib
#include "config.h"
#include "i106_stdint.h"
#include "irig106ch10.h"
#include "i106_time.h"
#include "i106_decode_tmats.h"
#include "i106_decode_tmats_r.h"
#include "i106_decode_pcmf1.h"

#include "Common.h"
#include "Ch10Writer.h"
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

// Initialize the Ch 10 header and the PCM Channel Specific Data Word. For now
// this writer class only supports PCM packed mode. Other PCM modes may be
// supported in the future.

void ClCh10Writer_PCM::Init(int iHandle, unsigned int uChanID)
    {
    this->iHandle = iHandle;
    this->uChanID = uChanID;

    // Setup the Ch 10 header
    iHeaderInit(&(suWriteMsgPCM.suCh10Header), uChanID, I106CH10_DTYPE_PCM_FMT_1, I106CH10_PFLAGS_CHKSUM_32 | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
    suWriteMsgPCM.suCh10Header.ubyHdrVer = CH10_VER_HDR_PCM;
    suWriteMsgPCM.suCh10Header.ulDataLen = 4;

    // Setup a buffer with enough memory to handle CSDW for now
    suWriteMsgPCM.uBuffLen = 1000;
    suWriteMsgPCM.suCh10Header.ulDataLen = 4;
    suWriteMsgPCM.pchDataBuff = (unsigned char *)malloc(suWriteMsgPCM.uBuffLen);
    suWriteMsgPCM.psuPCM_CSDW = (SuPcmF1_ChanSpec *)suWriteMsgPCM.pchDataBuff;

    // Init CSDW
    memset(suWriteMsgPCM.psuPCM_CSDW, 0, 4);
    suWriteMsgPCM.psuPCM_CSDW->bIntraPktHdr   = 1;  // Frame includes Intrapacket header
    suWriteMsgPCM.psuPCM_CSDW->bMajorFrInd    = 1;  // Start of major frame
    suWriteMsgPCM.psuPCM_CSDW->bMinorFrInd    = 1;  // Also start of minor frame
    suWriteMsgPCM.psuPCM_CSDW->uMajorFrStatus = 3;  // Locked
    suWriteMsgPCM.psuPCM_CSDW->uMinorFrStatus = 3;  // Locked
    suWriteMsgPCM.psuPCM_CSDW->bAlignment     = 0;  // 16 bit alignment
    suWriteMsgPCM.psuPCM_CSDW->bPackedMode    = 1;  // Packed mode

    // TMATS Channel Data Link Name for this channel
    std::stringstream   ssCDLN;
    ssCDLN << "PCMInChan" << uChanID;
    sCDLN = ssCDLN.str();

    }


// ----------------------------------------------------------------------------

// Return a string with the TMATS R section for this channel

std::string ClCh10Writer_PCM::TMATS(ClTmatsIndexes & TmatsIndex, std::string sDescription)
    {
    std::stringstream   ssTMATS;
    std::stringstream   ssDSI;

    if (sDescription != "")
        ssDSI << sDescription;
    else
        ssDSI << "PCMInChan" << uChanID;

    ssTMATS <<
        "R-" << TmatsIndex.iRIndex << "\\DSI-"  << TmatsIndex.iRSrcNum << ":" << ssDSI.str() << ";\n"
        "R-" << TmatsIndex.iRIndex << "\\TK1-"  << TmatsIndex.iRSrcNum << ":" << uChanID << ";\n"
        "R-" << TmatsIndex.iRIndex << "\\TK4-"  << TmatsIndex.iRSrcNum << ":" << uChanID << ";\n"
        "R-" << TmatsIndex.iRIndex << "\\CHE-"  << TmatsIndex.iRSrcNum << ":T;\n"
        "R-" << TmatsIndex.iRIndex << "\\CDT-"  << TmatsIndex.iRSrcNum << ":PCMIN;\n"
        "R-" << TmatsIndex.iRIndex << "\\PDTF-" << TmatsIndex.iRSrcNum << ":1;\n"
        "R-" << TmatsIndex.iRIndex << "\\PDP-"  << TmatsIndex.iRSrcNum << ":PFS;\n"
        "R-" << TmatsIndex.iRIndex << "\\CDLN-" << TmatsIndex.iRSrcNum << ":" << sCDLN << ";\n";

    // Get the P section stuff specific to the format
    ssTMATS << pSynthPcmFmt1->TMATS(TmatsIndex, sCDLN);

    return ssTMATS.str();
    } // end TMATS()

// ----------------------------------------------------------------------------

// Append a PCM frame/subframe to the end of a PCM packet.

void ClCh10Writer_PCM::AppendMsg(ClCh10Format_PCM_SynthFmt1 * psuPcmFrame)
    {
    unsigned        uCurrBufferOffset;

    // If there is no data yet then the packet RTC is the first message RTC
    if (suWriteMsgPCM.suCh10Header.ulDataLen <= 4)
        {
        // This assumes intra-packet time is in RTC format
        memcpy(suWriteMsgPCM.suCh10Header.aubyRefTime, psuPcmFrame->suIPH.aubyIntPktTime, 6);
        }

    uCurrBufferOffset = suWriteMsgPCM.suCh10Header.ulDataLen;

    // Expand the PCM packet buffer if necessary
    if (suWriteMsgPCM.psuPCM_CSDW->bIntraPktHdr == 1)
        suWriteMsgPCM.suCh10Header.ulDataLen += psuPcmFrame->uIPHLen;
    suWriteMsgPCM.suCh10Header.ulDataLen += psuPcmFrame->uFrameLen;

    if (suWriteMsgPCM.suCh10Header.ulDataLen > suWriteMsgPCM.uBuffLen)
        {
        suWriteMsgPCM.uBuffLen = suWriteMsgPCM.suCh10Header.ulDataLen + 1000;
        suWriteMsgPCM.pchDataBuff = (unsigned char *)realloc(suWriteMsgPCM.pchDataBuff, suWriteMsgPCM.uBuffLen);
        suWriteMsgPCM.psuPCM_CSDW = (SuPcmF1_ChanSpec *)suWriteMsgPCM.pchDataBuff;
        }
    
    // Build one of the various packet layouts

    // Intrapacket header
    if (suWriteMsgPCM.psuPCM_CSDW->bIntraPktHdr == 1)
        {
        // Adjust for 16 or 32 bit word size
        assert((psuPcmFrame->uIPHLen==10)||(psuPcmFrame->uIPHLen==12));
        memcpy(suWriteMsgPCM.pchDataBuff + uCurrBufferOffset, &(psuPcmFrame->suIPH), psuPcmFrame->uIPHLen);
        uCurrBufferOffset += psuPcmFrame->uIPHLen;
        }

    // Data
    memcpy(suWriteMsgPCM.pchDataBuff + uCurrBufferOffset, &(psuPcmFrame->suPcmFrame_Fmt1), psuPcmFrame->uFrameLen);
    uCurrBufferOffset += psuPcmFrame->uFrameLen;

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
