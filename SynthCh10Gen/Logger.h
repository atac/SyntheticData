#pragma once

#include <string>
#include <vector>
#include <time.h>

#include "LoggerTypes.h"

using namespace std;

class Logger
{
  typedef pair<time_t, LogItem> LogEntry;
  vector<LogEntry> log;

  unsigned int errorCount = 0;

public:
  const MessageCode DEFAULT_INFO = MessageCode("DEFAULT_INFO", CodeType::Info, "");
  const MessageCode DEFAULT_STATUS = MessageCode("DEFAULT_STATUS", CodeType::Status, "");
  const MessageCode DEFAULT_WARNING = MessageCode("DEFAULT_WARNING", CodeType::Warning, "");
  const MessageCode DEFAULT_ERROR = MessageCode("DEFAULT_ERROR", CodeType::Error, "");
    
  void Add(const MessageCode& code, string msg);
  void Add(LogItem msg);

  unsigned int LogSize();
  unsigned int ErrorCount();

  string ToString();
};
