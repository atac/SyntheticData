
// irig106lib
//#include "config.h"
//#include "i106_stdint.h"
//#include "irig106ch10.h"
//#include "i106_time.h"
//#include "i106_decode_arinc429.h"

#include "Common.h"
#include "Ch10Writer.h"
#include "SimState.h"

#include "Ch10Writer_A429.h"

using namespace Irig106;


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------


ClCh10Writer_A429::ClCh10Writer_A429()
    {
    }


ClCh10Writer_A429::~ClCh10Writer_A429()
    {
    }

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void ClCh10Writer_A429::Init(int iHandle, unsigned int uChanID)
    {
    this->iHandle = iHandle;
    this->uChanID = uChanID;

    // Setup the Ch 10 header
    iHeaderInit(&(suWriteMsgA429.suCh10Header), uChanID, I106CH10_DTYPE_ARINC_429_FMT_0, I106CH10_PFLAGS_CHKSUM_32 | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
    suWriteMsgA429.suCh10Header.ubyHdrVer = CH10_VER_HDR_A429;
    suWriteMsgA429.suCh10Header.ulDataLen = 4;

    // Setup a buffer with enough memory to handle CSDW for now
    suWriteMsgA429.uBuffLen = 4;
    suWriteMsgA429.suCh10Header.ulDataLen = 4;
    suWriteMsgA429.pchDataBuff = (unsigned char *)malloc(suWriteMsgA429.uBuffLen);
    suWriteMsgA429.psuA429CSDW = (SuArinc429F0_ChanSpec *)suWriteMsgA429.pchDataBuff;

    // Init CSDW
    memset(suWriteMsgA429.psuA429CSDW, 0, 4);
    suWriteMsgA429.psuA429CSDW->uMsgCount = 0;

    // TMATS Channel Data Link Name for this channel
    std::stringstream   ssCDLN;
    ssCDLN << "A429InChan" << uChanID;
    sCDLN = ssCDLN.str();

    }


// ----------------------------------------------------------------------------

// Return a string with the TMATS R section for this channel

std::string ClCh10Writer_A429::TMATS(ClTmatsIndexes & TmatsIndex, std::string sCDLN, int iTotalBuses, std::string sDescription)
    {
    std::stringstream   ssTMATS;
    std::stringstream   ssDSI;

    if (sDescription != "")
        ssDSI << sDescription;
    else
        ssDSI << "A429InChan" << uChanID;

    // Define the data source R record
    ssTMATS <<
        "R-" << TmatsIndex.iRIndex << "\\DSI-"    << TmatsIndex.iRSrcNum << ":" << ssDSI.str() << ";\n"
        "R-" << TmatsIndex.iRIndex << "\\TK1-"    << TmatsIndex.iRSrcNum << ":" << uChanID << ";\n"
        "R-" << TmatsIndex.iRIndex << "\\TK4-"    << TmatsIndex.iRSrcNum << ":" << uChanID << ";\n"
        "R-" << TmatsIndex.iRIndex << "\\CHE-"    << TmatsIndex.iRSrcNum << ":T;\n"
        "R-" << TmatsIndex.iRIndex << "\\ABTF-"   << TmatsIndex.iRSrcNum << ":0;\n"
        "R-" << TmatsIndex.iRIndex << "\\NAS\\N-" << TmatsIndex.iRSrcNum << ":1;\n"
        "R-" << TmatsIndex.iRIndex << "\\ASN-"    << TmatsIndex.iRSrcNum << "-1:1;\n"
        "R-" << TmatsIndex.iRIndex << "\\ANM-"    << TmatsIndex.iRSrcNum << "-1:" << ssDSI.str() << "-SubChan1;\n"
        "R-" << TmatsIndex.iRIndex << "\\CDT-"  << TmatsIndex.iRSrcNum << ":429IN;\n"
        "R-" << TmatsIndex.iRIndex << "\\CDLN-" << TmatsIndex.iRSrcNum << ":" << sCDLN << ";\n";

    // Define the bus B record
    CENTER_COMMENT(ssTMATS, 70, "**********************************************************************")
    CENTER_COMMENT(ssTMATS, 70, "ARINC 429 Bus Definitions")
    CENTER_COMMENT(ssTMATS, 70, (ssDSI.str().c_str()))
    CENTER_COMMENT(ssTMATS, 70, "**********************************************************************")

    ssTMATS <<
        "B-" << TmatsIndex.iBIndex << "\\DLN:" << sCDLN << ";\n"    // Link from R-x\CDLN-n above
        "B-" << TmatsIndex.iBIndex << "\\NBS\\N:" << iTotalBuses << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\BID-1:0;\n"
        "B-" << TmatsIndex.iBIndex << "\\BNA-1:" << ssDSI.str() << ";\n"
        "B-" << TmatsIndex.iBIndex << "\\BT-1:A429;\n";

    return ssTMATS.str();
    } // end TMATS()

// ----------------------------------------------------------------------------

// Append an ARINC 429 message to the end of a ARINC 429 packet.

void ClCh10Writer_A429::AppendMsg(ClCh10Format_ARINC429* pA429Msg)
    {
    unsigned        uCurrBufferOffset;

    // Return of no messages to copy
    if (pA429Msg->aArinc429Msgs.size() == 0)
        return;

    assert(pA429Msg->aArinc429Msgs[0].suIPH.uGapTime == 0);

    // If this is the first message then the packet RTC is the first message RTC
    if (suWriteMsgA429.psuA429CSDW->uMsgCount == 0)
        {
        vLLInt2TimeArray(&(pA429Msg->llRefTime), suWriteMsgA429.suCh10Header.aubyRefTime);
        }

    // If this isn't the first message then the firt gap time need to be fixed
    else
        {
        uint32_t    uFirstGapTime;
        uFirstGapTime = (uint32_t)(pA429Msg->llRefTime - lPrevMessageTime) - uPrevGapSum;
        pA429Msg->aArinc429Msgs[0].suIPH.uGapTime = uFirstGapTime;
        } // end if not first message

    // Update the buffer offset pointer and message count
    uCurrBufferOffset = suWriteMsgA429.suCh10Header.ulDataLen;
    suWriteMsgA429.psuA429CSDW->uMsgCount += pA429Msg->aArinc429Msgs.size();

    // Expand the ARINC 429 packet buffer if necessary
    suWriteMsgA429.suCh10Header.ulDataLen =   
             sizeof(SuArinc429F0_ChanSpec) +
            (sizeof(ClCh10Format_ARINC429::SuArinc429Msg) * suWriteMsgA429.psuA429CSDW->uMsgCount);
            
    if (suWriteMsgA429.suCh10Header.ulDataLen > suWriteMsgA429.uBuffLen)
        {
        suWriteMsgA429.uBuffLen = suWriteMsgA429.suCh10Header.ulDataLen + 1000;
        suWriteMsgA429.pchDataBuff = (unsigned char *)realloc(suWriteMsgA429.pchDataBuff, suWriteMsgA429.uBuffLen);
        suWriteMsgA429.psuA429CSDW = (SuArinc429F0_ChanSpec *)suWriteMsgA429.pchDataBuff;
        }

    // Copy ARINC 429 messages into the write buffer
    memcpy(&(suWriteMsgA429.pchDataBuff[uCurrBufferOffset]), 
           &(pA429Msg->aArinc429Msgs[0]), 
           sizeof(ClCh10Format_ARINC429::SuArinc429Msg) * pA429Msg->aArinc429Msgs.size());

    // Store the RTC and sum of gap times for use on the next set of messages
    lPrevMessageTime = pA429Msg->llRefTime;
    uPrevGapSum = 0;
    for (int iIdx=1; iIdx < pA429Msg->aArinc429Msgs.size(); iIdx++)
        uPrevGapSum += pA429Msg->aArinc429Msgs[iIdx].suIPH.uGapTime;

    } // end AppendMsg()


// ----------------------------------------------------------------------------

void ClCh10Writer_A429::Commit()
    {

    uint32_t    uDataBuffLen;

    // Make sure the data buffer is big enough to hold the filler and checksum
    int     iChecksumType = suWriteMsgA429.suCh10Header.ubyPacketFlags & I106CH10_PFLAGS_CHKSUM_MASK;
    uDataBuffLen = uCalcDataBuffReqSize(suWriteMsgA429.suCh10Header.ulDataLen, iChecksumType);

    if (uDataBuffLen > suWriteMsgA429.uBuffLen)
        {
        suWriteMsgA429.uBuffLen += 1000;
        suWriteMsgA429.pchDataBuff = (unsigned char *)realloc(suWriteMsgA429.pchDataBuff, suWriteMsgA429.uBuffLen);
        suWriteMsgA429.psuA429CSDW = (SuArinc429F0_ChanSpec *)suWriteMsgA429.pchDataBuff;
        }

    // Put a checksum on the end of the packet
    uAddDataFillerChecksum(&(suWriteMsgA429.suCh10Header), suWriteMsgA429.pchDataBuff);

    // Make the header checksum
    suWriteMsgA429.suCh10Header.uChecksum = uCalcHeaderChecksum(&(suWriteMsgA429.suCh10Header));

    // Write it
    enI106Ch10WriteMsg(iHandle, &(suWriteMsgA429.suCh10Header), suWriteMsgA429.pchDataBuff);

    // Reset the buffer
    suWriteMsgA429.suCh10Header.ubySeqNum++;
    suWriteMsgA429.suCh10Header.ulDataLen = 4;
    suWriteMsgA429.psuA429CSDW->uMsgCount = 0;

    return;
    } // Commit()
