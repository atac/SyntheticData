#include "Ch10Format_Video.h"

Ch10Format_Video::Ch10Format_Video(CSV_FIELDS dataLabels, CSV_FIELDS dataTypes)
{
  // This module is designed to format a single video stream and will use the
  // first BLOB column encountered.
  FilterFields(dataLabels, dataTypes);
}


void Ch10Format_Video::SetRTC(int64_t* pllRelTime)
{
  this->pllRelTime = pllRelTime;
}

void Ch10Format_Video::FormatMsg(ClSimState* simState) 
{
  videoData = simState->blobState.at(fieldName);
}

std::string Ch10Format_Video::TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN) 
{
  return "";
}

void Ch10Format_Video::FilterFields(CSV_FIELDS labels, CSV_FIELDS types)
{
  if (labels.size() != types.size())
    return;

  for (int i = 0; i < labels.size(); i++) {
    if (types[i] == "BLOB")
    {
      fieldName = labels[i];
      break;
    }
  }
}