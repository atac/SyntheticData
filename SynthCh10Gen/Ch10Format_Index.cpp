#include "Ch10Format_Index.h"

Ch10Format_Index::Ch10Format_Index()
{
  pCh10Header = nullptr;
}

Ch10Format_Index::~Ch10Format_Index()
{
}


void Ch10Format_Index::Init(SuI106Ch10Header* psuHeader) {
  this->pCh10Header = psuHeader;
}

void Ch10Format_Index::FormatMsg(ClSimState* simState = nullptr) {
  memcpy(formatValues.refTime, pCh10Header->aubyRefTime, 6);
  formatValues.packetLen = pCh10Header->ulPacketLen;
  formatValues.chID = pCh10Header->uChID;
  formatValues.dataType = pCh10Header->ubyDataType;
}

Ch10Format_Index::RequiredIndexFields Ch10Format_Index::GetFormatValues() {
  return formatValues;
}
