#pragma once

#include <string>
#include <map>
#include <sstream>

using namespace std;

enum class CodeType : unsigned int {
  INFO = 0,
  STATUS,
  WARNING,
  ERROR
};

static const map<CodeType, string> codeTypeStringMap = {
  { CodeType::INFO, "INFO" },
  { CodeType::STATUS, "STATUS" },
  { CodeType::WARNING, "WARNING" },
  { CodeType::ERROR, "ERROR" }
};

class MessageCode {
public:
  MessageCode(string name, CodeType type, string description)
    : name(name), type(type), desc(description) {};

  inline CodeType GetType() { return type; }

  friend inline ostream& operator<<(ostream& os, const MessageCode& obj) {
    os << "(" << codeTypeStringMap.at(obj.type) << ") " << obj.name;
    if (!obj.desc.empty())
      os << ": " << obj.desc;
    return os;
  }

private:
  string name;
  CodeType type;
  string desc;
};

class LogItem {
public:
  LogItem(const MessageCode& code, string message)
    : code(code), msg(message) {};

  inline CodeType GetType() { return code.GetType(); }

  friend inline ostream& operator<<(ostream& os, const LogItem& obj) {
    os << obj.code << endl;
    os << obj.msg;
    return os;
  }
private:
  MessageCode code;
  string msg;
};
