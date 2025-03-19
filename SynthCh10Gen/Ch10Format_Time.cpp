#include "Ch10Format_Time.h"



void Ch10Format_Time::FormatMsg(ClSimState* simState) {
  currentTime = simState->GetCurrSimClockTime();
}

double Ch10Format_Time::GetCurrentTime() {
  return currentTime;
}

// ----------------------------------------------------------------------------

// Return a string with the TMATS R section for this channel

std::string Ch10Format_Time::TMATS(ClTmatsIndexes& TmatsIndex, std::string CDLN)
{
  return "";
} // end TMATS()