#pragma once

#include <string>
#include <vector>
#include <time.h>

#include "LoggerTypes.h"

using namespace std;

namespace MyErrorCodeSet {
  const MessageCode FIRST_ERROR("FIRST_ERROR", CodeType::ERROR, "My description");
}

class Logger
{
  typedef pair<time_t, LogItem> LogEntry;
  vector<LogEntry> log;

  unsigned int errorCount = 0;

public:
  const MessageCode DEFAULT_INFO = MessageCode("DEFAULT_INFO", CodeType::INFO, "");
  const MessageCode DEFAULT_STATUS = MessageCode("DEFAULT_STATUS", CodeType::STATUS, "");
  const MessageCode DEFAULT_WARNING = MessageCode("DEFAULT_WARNING", CodeType::WARNING, "");
  const MessageCode DEFAULT_ERROR = MessageCode("DEFAULT_ERROR", CodeType::ERROR, "");
    
  void Add(MessageCode& code, string msg);
  void Add(LogItem msg);

  unsigned int LogSize();
  unsigned int ErrorCount();

  string ToString();
};
