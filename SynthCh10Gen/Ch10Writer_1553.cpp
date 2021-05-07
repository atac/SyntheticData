#include "Ch10Writer_1553.h"


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClCh10Writer_1553::ClCh10Writer_1553()
    {
    }


ClCh10Writer_1553::~ClCh10Writer_1553()
    {
    }

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void ClCh10Writer_1553::Init(int iHandle, unsigned int uChanID)
    {
    this->iHandle = iHandle;
    this->uChanID = uChanID;

    // Setup the Ch 10 header
    iHeaderInit(&(suWriteMsg1553.suCh10Header), uChanID, I106CH10_DTYPE_1553_FMT_1, I106CH10_PFLAGS_CHKSUM_32 | I106CH10_PFLAGS_TIMEFMT_IRIG106, 0);
    suWriteMsg1553.suCh10Header.ubyHdrVer = 3;
    suWriteMsg1553.suCh10Header.ulDataLen = 4;

    // Setup a buffer with enough memory to handle CSDW for now
    suWriteMsg1553.uBuffLen = 4;
    suWriteMsg1553.suCh10Header.ulDataLen = 4;
    suWriteMsg1553.pchDataBuff = (unsigned char *)malloc(suWriteMsg1553.uBuffLen);
    suWriteMsg1553.psu1553CSDW = (Su1553F1_ChanSpec *)suWriteMsg1553.pchDataBuff;

    // Init CSDW
    memset(suWriteMsg1553.psu1553CSDW, 0, 4);
    suWriteMsg1553.psu1553CSDW->uTTB    = 1;    // Time tag is first bit of first word
    suWriteMsg1553.psu1553CSDW->uMsgCnt = 0;

    // TMATS Channel Data Link Name for this channel
    std::stringstream   ssCDLN;
    ssCDLN << "1553InChan" << uChanID;
    sCDLN = ssCDLN.str();

    }


// ----------------------------------------------------------------------------

// Return a string with the TMATS R section for this channel

std::string ClCh10Writer_1553::TMATS(int iRSection, int iEnumN)
    {
    std::stringstream   ssTMATS;

    ssTMATS <<
        "R-" << iRSection << "\\TK1-"  << iEnumN << ":" << uChanID << ";\n"
        "R-" << iRSection << "\\TK4-"  << iEnumN << ":" << uChanID << ";\n"
        "R-" << iRSection << "\\CHE-"  << iEnumN << ":T;\n"
        "R-" << iRSection << "\\DSI-"  << iEnumN << ":1553InChan" << uChanID << ";\n"
        "R-" << iRSection << "\\CDT-"  << iEnumN << ":1553IN;\n"
        "R-" << iRSection << "\\CDLN-" << iEnumN << ":" << sCDLN << ";\n";

    return ssTMATS.str();
    } // end TMATS()

// ----------------------------------------------------------------------------

// Append a 1553 message to the end of a 1553 packet.
// No status response is indicated by Status Word = -1
// The message length value in the IPH needs to be correct, even for Mode Codes.

void ClCh10Writer_1553::AppendMsg(ClCh10Format_1553 * psu1553Msg)
    {
    AppendMsg(&(psu1553Msg->suIPH), psu1553Msg->suCmdWord1.uValue, psu1553Msg->suStatWord1.uValue, psu1553Msg->suCmdWord2.uValue, psu1553Msg->suStatWord2.uValue, psu1553Msg->auData);
    }


void ClCh10Writer_1553::AppendMsg(Su1553F1_Header * psu1553IPH, int32_t iCmdWord1, int32_t iStatWord1, int32_t iCmdWord2, int32_t iStatWord2, uint16_t auData[])
    {
    unsigned        uCurrBufferOffset;
    unsigned        uDataBytes;
    SuCmdWordU      suCmdWord1;
    SuCmdWordU      suCmdWord2;
    SuStatWordU     suStatWord1;
    SuStatWordU     suStatWord2;

    // If this is the first message then the packet RTC is the first message RTC
    if (suWriteMsg1553.psu1553CSDW->uMsgCnt == 0)
        {
        // This assumes intra-packet time is in RTC format
        memcpy(suWriteMsg1553.suCh10Header.aubyRefTime, psu1553IPH->aubyIntPktTime, 6);
        }

    suCmdWord1.uValue  = (uint16_t)(iCmdWord1  & 0x0000ffff);
    suCmdWord2.uValue  = (uint16_t)(iCmdWord2  & 0x0000ffff);
    suStatWord1.uValue = (uint16_t)(iStatWord1 & 0x0000ffff);
    suStatWord2.uValue = (uint16_t)(iStatWord2 & 0x0000ffff);

    uCurrBufferOffset = suWriteMsg1553.suCh10Header.ulDataLen;
    suWriteMsg1553.psu1553CSDW->uMsgCnt++;

    // Expand the 1553 packet buffer if necessary
    suWriteMsg1553.suCh10Header.ulDataLen += sizeof(Su1553F1_Header) + psu1553IPH->uMsgLen;
    if (suWriteMsg1553.suCh10Header.ulDataLen > suWriteMsg1553.uBuffLen)
        {
        suWriteMsg1553.uBuffLen += 1000;
        suWriteMsg1553.pchDataBuff = (unsigned char *)realloc(suWriteMsg1553.pchDataBuff, suWriteMsg1553.uBuffLen);
        suWriteMsg1553.psu1553CSDW = (Su1553F1_ChanSpec *)suWriteMsg1553.pchDataBuff;
        }
    
    // Build one of the various packet layouts

    // Regular message
    if (psu1553IPH->bRT2RT == 0)
        // Transmit command but no status response, so no data either
        if ((suCmdWord1.suStruct.bTR == 1) && (iStatWord1 < 0))
            {
            // Intrapacket header
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, psu1553IPH, sizeof(Su1553F1_Header));
            uCurrBufferOffset += sizeof(Su1553F1_Header);

            // Command Word 1
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord1.uValue, 2);
            uCurrBufferOffset += 2;
            } // end if no transmit status or data

        // Receive command but no status response
        else if ((suCmdWord1.suStruct.bTR == 0) && (iStatWord1 < 0))
            {
            // Intrapacket header
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, psu1553IPH, sizeof(Su1553F1_Header));
            uCurrBufferOffset += sizeof(Su1553F1_Header);

            // Command Word 1
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord1.uValue, 2);
            uCurrBufferOffset += 2;

            // Data
            uDataBytes = psu1553IPH->uMsgLen - 2;
            if (uDataBytes > 0)
                {
                memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, auData, uDataBytes);
                uCurrBufferOffset += uDataBytes;
                }
            } // end if no receive status

        // Full message
        else
            {
            // Intrapacket header
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, psu1553IPH, sizeof(Su1553F1_Header));
            uCurrBufferOffset += sizeof(Su1553F1_Header);

            // Command Word 1
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord1.uValue, 2);
            uCurrBufferOffset += 2;

            // Transmit command
            if (suCmdWord1.suStruct.bTR == 1)
                {
                // Status Word 1
                memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suStatWord1.uValue, 2);
                uCurrBufferOffset += 2;

                // Data
                uDataBytes = psu1553IPH->uMsgLen - 4;
                if (uDataBytes > 0)
                    {
                    memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, auData, uDataBytes);
                    uCurrBufferOffset += uDataBytes;
                    }
                }

            // Receive command
            else
                {
                // Data
                uDataBytes = psu1553IPH->uMsgLen - 4;
                if (uDataBytes > 0)
                    {
                    memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, auData, uDataBytes);
                    uCurrBufferOffset += uDataBytes;
                    }

                // Status Word 1
                memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suStatWord1.uValue, 2);
                uCurrBufferOffset += 2;
                }

            } // end if full message

    // RT to RT
    else
        // No transmit status response so only command words
        if (iStatWord2 < 0)
            {
            // Intrapacket header
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, psu1553IPH, sizeof(Su1553F1_Header));
            uCurrBufferOffset += sizeof(Su1553F1_Header);

            // Command Word 1
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord1.uValue, 2);
            uCurrBufferOffset += 2;

            // Command Word 2
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord2.uValue, 2);
            uCurrBufferOffset += 2;
            } // end if RT to RT, no transmit status or data

        // No receive status word so commands words, transmit status, and data
        else if ((iStatWord2 >= 0) && (iStatWord1 < 0))
            {
            // Intrapacket header
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, psu1553IPH, sizeof(Su1553F1_Header));
            uCurrBufferOffset += sizeof(Su1553F1_Header);

            // Command Word 1
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord1.uValue, 2);
            uCurrBufferOffset += 2;

            // Command Word 2
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord2.uValue, 2);
            uCurrBufferOffset += 2;

            // Status Word 2
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suStatWord2.uValue, 2);
            uCurrBufferOffset += 2;

            // Data
            uDataBytes = psu1553IPH->uMsgLen - 6;
            if (uDataBytes > 0)
                {
                memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, auData, uDataBytes);
                uCurrBufferOffset += uDataBytes;
                }
            } // end if RT to RT, no receive status

        // Full RT to RT message
        else
            {
            // Intrapacket header
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, psu1553IPH, sizeof(Su1553F1_Header));
            uCurrBufferOffset += sizeof(Su1553F1_Header);

            // Command Word 1
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord1.uValue, 2);
            uCurrBufferOffset += 2;

            // Command Word 2
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suCmdWord2.uValue, 2);
            uCurrBufferOffset += 2;

            // Status Word 2
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suStatWord2.uValue, 2);
            uCurrBufferOffset += 2;

            // Data
            uDataBytes = psu1553IPH->uMsgLen - 8;
            if (uDataBytes > 0)
                {
                memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, auData, uDataBytes);
                uCurrBufferOffset += uDataBytes;
                }

            // Status Word 1
            memcpy(suWriteMsg1553.pchDataBuff + uCurrBufferOffset, &suStatWord1.uValue, 2);
            uCurrBufferOffset += 2;
            } // end if full RT to RT

    } // end WriteMsg1553Append()


// ----------------------------------------------------------------------------

void ClCh10Writer_1553::Commit()
    {
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

    return;
    } // Commit()
