#include "Ch10Format_Video.h"

Ch10Format_Video::Ch10Format_Video(FieldSet fields)
{
  // This module is designed to format a single video stream and will use the
  // first BLOB column encountered.
  FilterFields(fields);
}


void Ch10Format_Video::SetRTC(int64_t relTime)
{
  this->llRelTime = relTime;
}

void Ch10Format_Video::FormatMsg(ClSimState* simState) 
{
  videoData = simState->blobState.at(fieldName);
}

std::string Ch10Format_Video::TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN) 
{
  return "";
}

void Ch10Format_Video::FilterFields(FieldSet fields)
{
  for (auto f = fields.begin(); f != fields.end(); f++) {
    if (f->getType() == FieldType::BLOB_FIELD)
    {
      videoField = (*f);
      break;
    }
  }
}