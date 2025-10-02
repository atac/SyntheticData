#include "Logger.h"

void Logger::Add(const MessageCode& code, string msg) {
  Add(LogItem(code, msg));
}

void Logger::Add(LogItem entry) {
  if (entry.GetType() == CodeType::Error)
    errorCount++;

  log.push_back(pair(time(nullptr), entry));
}

unsigned int Logger::LogSize() {
  return log.size();
}

unsigned int Logger::ErrorCount() {
  return errorCount;
}

string Logger::ToString() {
  ostringstream os;
  os << "Log Messages (" << LogSize() << ")\n\n";
  for (int i = 0; i < log.size(); i++) {
    struct tm t = *localtime(&log[i].first);
    os << i+1 << "  ";// << t.tm_hour << ":" << t.tm_min << ":" << t.tm_sec << endl;
    os << log[i].second << endl << endl;
  }

  return os.str();
}
