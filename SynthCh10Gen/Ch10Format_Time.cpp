#include "Ch10Format_Time.h"



void Ch10Format_Time::FormatMsg(ClSimState* simState) {
  currentTime = simState->GetCurrSimClockTime();
}

double Ch10Format_Time::GetCurrentTime() {
  return currentTime;
}

// ----------------------------------------------------------------------------

// Return a string with the TMATS R section for this channel

std::string Ch10Format_Time::TMATS(ClTmatsIndexes& TmatsIndex, std::string CDLN, int channelID)
{
  std::stringstream   ssTMATS;

  ssTMATS <<
    "R-" << TmatsIndex.iRIndex << "\\DSI-" << TmatsIndex.iRSrcNum << ":" << CDLN << ";\n"
    "R-" << TmatsIndex.iRIndex << "\\TK1-" << TmatsIndex.iRSrcNum << ":" << channelID << ";\n"
    "R-" << TmatsIndex.iRIndex << "\\TK4-" << TmatsIndex.iRSrcNum << ":" << channelID << ";\n"
    "R-" << TmatsIndex.iRIndex << "\\CHE-" << TmatsIndex.iRSrcNum << ":T;\n"
    "R-" << TmatsIndex.iRIndex << "\\CDT-" << TmatsIndex.iRSrcNum << ":TIMEIN;\n"
    "R-" << TmatsIndex.iRIndex << "\\CDLN-" << TmatsIndex.iRSrcNum << ":" << CDLN << ";\n"
    "R-" << TmatsIndex.iRIndex << "\\TTF-" << TmatsIndex.iRSrcNum << ":1;\n"
    "R-" << TmatsIndex.iRIndex << "\\TFMT-" << TmatsIndex.iRSrcNum << ":I;\n"
    "R-" << TmatsIndex.iRIndex << "\\TSRC-" << TmatsIndex.iRSrcNum << ":I;\n";

  return ssTMATS.str();
} // end TMATS()