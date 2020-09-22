
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
#include "i106_decode_index.h"
#include "i106_index.h"

#include "Ch10Writer_Index.h"

// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClCh10Writer_Index::ClCh10Writer_Index()
    {
    }


ClCh10Writer_Index::~ClCh10Writer_Index()
    {
    }

// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void ClCh10Writer_Index::Init(int iHandle, unsigned int uChanID)
    {
    this->iHandle                  = iHandle;
    this->uChanID                  = uChanID;
    this->uSeqNum                  = 0;
    this->suPrevRootPacket.lOffset = -1;
    }


// ----------------------------------------------------------------------------

// Add an index entry into the in-memory index structure

void ClCh10Writer_Index::AppendNodeIndex(SuI106Ch10Header * psuHeader)
    {
    __int64             uOffset;
    SuIndex_NodeMsg     suNodeInfo;

    enI106Ch10GetPos(iHandle, &uOffset);

    memset(&suNodeInfo, 0, sizeof(SuIndex_NodeMsg));
    memcpy(suNodeInfo.suTime.suRtcTime.aubyRefTime, psuHeader->aubyRefTime, 6);
    suNodeInfo.lOffset           = uOffset - psuHeader->ulPacketLen;
    suNodeInfo.suData.uChannelID = psuHeader->uChID;
    suNodeInfo.suData.uDataType  = psuHeader->ubyDataType;

    asuNodeData.push_back(suNodeInfo);
    }


// ----------------------------------------------------------------------------

void ClCh10Writer_Index::WriteNodePacket()
    {
    SuI106Ch10Header    suCh10Header;
    __int64             uOffset;
    SuIndex_ChanSpec    suIndexCSDW;
//    SuIndex_Time        suIndexTime;
    unsigned char       achTrailerBuffer[10];
    int                 iTrailerBufferLen = 10;

    // If no nodes then nothing to do
    if (asuNodeData.size() <= 0)
        return;

    // Make an entry into the root packet table
    enI106Ch10GetPos(iHandle, &uOffset);
    AppendRootIndex(&(asuNodeData[0].suTime), uOffset);

    // Setup the node packet Channel Specific Data Word
    memset(&suIndexCSDW, 0, sizeof(SuIndex_ChanSpec));
    suIndexCSDW.uIndexType   = 1;           // Node
    suIndexCSDW.bFileSize    = 0;           // File size not present
    suIndexCSDW.bIntraPktHdr = 0;           // No Intrapacket Data Header
    suIndexCSDW.uIdxEntCount = asuNodeData.size();  //  Numbe of node indexes

    // Write the node packet to the Ch 10 file
    iHeaderInit(&suCh10Header, uChanID, I106CH10_DTYPE_RECORDING_INDEX, I106CH10_PFLAGS_CHKSUM_NONE | I106CH10_PFLAGS_TIMEFMT_IRIG106, uSeqNum);
    suCh10Header.ulDataLen = sizeof(SuIndex_ChanSpec) + (asuNodeData.size() * sizeof(SuIndex_NodeMsg));
    suCh10Header.ubyHdrVer = 6;
    memcpy(suCh10Header.aubyRefTime, &(asuNodeData[0].suTime.suRtcTime), 6);
    uAddDataFillerChecksum2(&suCh10Header, &suIndexCSDW, 4, (unsigned char *)asuNodeData.data(), achTrailerBuffer, &iTrailerBufferLen);
    suCh10Header.uChecksum = uCalcHeaderChecksum(&suCh10Header);

    enI106Ch10WriteMsg2(iHandle, &suCh10Header, &suIndexCSDW, 4, (unsigned char *)asuNodeData.data(), achTrailerBuffer, iTrailerBufferLen);

    // Get ready for the next node packet
    asuNodeData.clear();
    uSeqNum++;

    } // end WriteNodePacket()


// ----------------------------------------------------------------------------

void ClCh10Writer_Index::AppendRootIndex(SuIndex_Time * psuNodeIndexTime, __int64 llNodeIndexOffset)
    {
    SuIndex_RootMsg     suRootData;

    memcpy(&(suRootData.suTime), psuNodeIndexTime, sizeof(SuIndex_Time));
    suRootData.lOffset = llNodeIndexOffset;

    asuRootData.push_back(suRootData);
    }


// ----------------------------------------------------------------------------

void ClCh10Writer_Index::WriteRootPacket()
    {
    __int64             uOffset;
    SuI106Ch10Header    suCh10Header;
    SuIndex_ChanSpec    suIndexCSDW;
//    SuIndex_Time        suIndexTime;
    unsigned char       achTrailerBuffer[10];
    int                 iTrailerBufferLen = 10;

    // Get our current offset
    enI106Ch10GetPos(iHandle, &uOffset);

    // If this is the first root packet then have the previous root packet
    // offset data point back to itself.
    if (suPrevRootPacket.lOffset < 0)
        {
        suPrevRootPacket.lOffset = uOffset;
        memcpy(&(suPrevRootPacket.suTime), &(asuRootData[0].suTime), sizeof(SuIndex_Time));
        }

    // Add the pointer to the previous root packet
    asuRootData.push_back(suPrevRootPacket);

    // Setup the root packet Channel Specific Data Word
    memset(&suIndexCSDW, 0, sizeof(SuIndex_ChanSpec));
    suIndexCSDW.uIndexType   = 0;           // Root
    suIndexCSDW.bFileSize    = 0;           // File size not present
    suIndexCSDW.bIntraPktHdr = 0;           // No Intrapacket Data Header
    suIndexCSDW.uIdxEntCount = asuRootData.size();  //  Numbe of root indexes

    // Write the root packet to the Ch 10 file
    iHeaderInit(&suCh10Header, uChanID, I106CH10_DTYPE_RECORDING_INDEX, I106CH10_PFLAGS_CHKSUM_NONE | I106CH10_PFLAGS_TIMEFMT_IRIG106, uSeqNum);
    suCh10Header.ulDataLen = sizeof(SuIndex_ChanSpec) + (asuRootData.size() * sizeof(SuIndex_RootMsg));
    suCh10Header.ubyHdrVer = 6;
    memcpy(suCh10Header.aubyRefTime, &(asuRootData[0].suTime.suRtcTime), 6);
    uAddDataFillerChecksum2(&suCh10Header, &suIndexCSDW, 4, (unsigned char *)asuRootData.data(), achTrailerBuffer, &iTrailerBufferLen);
    suCh10Header.uChecksum = uCalcHeaderChecksum(&suCh10Header);

    enI106Ch10WriteMsg2(iHandle, &suCh10Header, &suIndexCSDW, 4, (unsigned char *)asuRootData.data(), achTrailerBuffer, iTrailerBufferLen);

    // Get ready for the next root packet
    suPrevRootPacket.lOffset = uOffset;
    memcpy(&(suPrevRootPacket.suTime), &(asuRootData[0].suTime), sizeof(SuIndex_Time));
    asuRootData.clear();

    uSeqNum++;

    }