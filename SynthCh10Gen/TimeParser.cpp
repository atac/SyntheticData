#include "TimeParser.h"



TimeParser::TimeParser()
{
  format = Format::INVALID;
}

TimeParser::~TimeParser()
{
}

bool TimeParser::Valid()
{
  return format != Format::INVALID;
}

void TimeParser::Init(std::string timeString, Units units)
{
  this->units = units;
  format = DetermineFormat(timeString);
  SetParseMethod();
}

void TimeParser::Init(Format format, Units units)
{
  this->units = units;
  this->format = format;
}

bool TimeParser::Parse(std::string timeString, double& timeSeconds)
{
  if (time == nullptr)
    return false;

  return (this->*DoParse)(timeString, timeSeconds);
}

TimeParser::Format TimeParser::GetFormat()
{
  return this->format;
}



void TimeParser::SetParseMethod()
{
  switch (this->format)
  {
  case Format::NANOSECONDS:
  case Format::MICROSECONDS:
  case Format::MILLISECONDS:
  case Format::SECONDS:
    DoParse = &TimeParser::Parse_Double;
    break;
  case Format::MM_SS:
    DoParse = &TimeParser::Parse_MM_SS;
    break;
  case Format::HH_MM_SS:
    DoParse = &TimeParser::Parse_HH_MM_SS;
    break;
  case Format::DDD_HH_MM_SS:
    DoParse = &TimeParser::Parse_DDD_HH_MM_SS;
    break;
  case Format::MM_DD_HH_MM_SS:
    DoParse = &TimeParser::Parse_MM_DD_HH_MM_SS;
    break;
  case Format::YYYY_DDD_HH_MM_SS:
    DoParse = &TimeParser::Parse_YYYY_DDD_HH_MM_SS;
    break;
  case Format::YYYY_MM_DD_HH_MM_SS:
    DoParse = &TimeParser::Parse_YYYY_MM_DD_HH_MM_SS;
    break;
  default:
    DoParse = &TimeParser::Parse_INVALID;
    break;
  }
}

bool TimeParser::Parse_INVALID(std::string sTime, double& time)
{
  return false;
}

bool TimeParser::Parse_Double(std::string sTime, double& time)
{
  try {
    time = std::stod(sTime);

    switch (this->units) {
    case Units::DAYS:
      time *= 86400;
      break;
    case Units::SECONDS:
    default:
      break;
    }
  }
  catch (...) {
    return false;
  }

  return true;
}

bool TimeParser::Parse_MM_SS(std::string sTime, double& time)
{
  tm t;
  time_t tt;
  double sec;
  int tok;
  
  tok = sscanf(sTime.c_str(), "%02d:%lf",
    &t.tm_min, &sec);

  if (tok != 2)
    return false;

  t.tm_year = 0;
  t.tm_yday = 0;
  t.tm_hour = 0;
  t.tm_sec = (int)sec;

  tt = _mkgmtime(&t);

  time = tt + (sec - t.tm_sec);

  return true;
}

bool TimeParser::Parse_HH_MM_SS(std::string sTime, double& time)
{
  tm t;
  time_t tt;
  double sec;
  int tok;

  tok = sscanf(sTime.c_str(), "%02d:%02d:%lf",
    &t.tm_hour, &t.tm_min, &sec);

  if (tok != 3)
    return false;

  t.tm_year = 0;
  t.tm_yday = 0;
  t.tm_sec = (int)sec;

  tt = _mkgmtime(&t);

  time = tt + (sec - t.tm_sec);

  return true;
}

bool TimeParser::Parse_DDD_HH_MM_SS(std::string sTime, double& time)
{
  tm t;
  time_t tt;
  double sec;
  int tok;

  tok = sscanf(sTime.c_str(), "%03d%*1[: ]%02d:%02d:%lf",
    &t.tm_yday, &t.tm_hour, &t.tm_min, &sec);

  if (tok != 4)
    return false;

  t.tm_year = 0;
  t.tm_sec = (int)sec;

  tt = _mkgmtime(&t);

  time = tt + (sec - t.tm_sec);

  return true;
}
bool TimeParser::Parse_MM_DD_HH_MM_SS(std::string sTime, double& time)
{
  tm t;
  time_t tt;
  double sec;
  int tok;

  tok = sscanf(sTime.c_str(), "%02d%*1[-: ]%02d%*1[: ]%02d:%02d:%lf",
    &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &sec);

  if (tok != 5)
    return false;

  t.tm_year = 0;
  t.tm_mon -= 1;
  t.tm_sec = (int)sec;

  tt = _mkgmtime(&t);

  time = tt + (sec - t.tm_sec);

  return true;
}

bool TimeParser::Parse_YYYY_DDD_HH_MM_SS(std::string sTime, double& time)
{
  tm t;
  time_t tt;
  double sec;
  int tok;

  tok = sscanf(sTime.c_str(), "%04d%*1[-: ]%03d%*1[: ]%02d:%02d:%lf",
    &t.tm_year, &t.tm_yday, &t.tm_hour, &t.tm_min, &sec);

  if (tok != 5)
    return false;

  t.tm_year -= 1900;
  t.tm_sec = (int)sec;

  tt = _mkgmtime(&t);

  time = tt + (sec - t.tm_sec);

  return true;
}

bool TimeParser::Parse_YYYY_MM_DD_HH_MM_SS(std::string sTime, double& time)
{
  tm t;
  time_t tt;
  double sec;
  int tok;

  tok = sscanf(sTime.c_str(), "%04d%*1[-: ]%02d%*1[-: ]%02d%*1[: ]%02d:%02d:%lf",
    &t.tm_year, &t.tm_mon, &t.tm_mday, &t.tm_hour, &t.tm_min, &sec);

  if (tok != 6)
    return false;

  t.tm_year -= 1900;
  t.tm_mon -= 1;
  t.tm_sec = (int)sec;

  tt = _mkgmtime(&t);

  time = tt + (sec - t.tm_sec);

  return true;
}


TimeParser::Units TimeParser::GetTimeUnitFromString(std::string unitStr) {
  TimeParser::Units u = TimeParser::Units::INVALID;

  transform(unitStr.begin(), unitStr.end(), unitStr.begin(), ::tolower);

  if (unitStr == "s" || unitStr == "sec" || unitStr == "seconds")
    u = TimeParser::Units::SECONDS;
  else if (unitStr == "d" || unitStr == "day" || unitStr == "days")
    u = TimeParser::Units::DAYS;

  return u;
}

TimeParser::Format TimeParser::DetermineFormat(std::string timeString)
{
  /*
   * Regex key for readability:
   *     \\d{2}         : exactly 2 digits
   *    (\\.\\d+){0,1}  : optional decimal portion of a number
   *    (:|\\-|\\s)     : matches ':', '-', or whitespace
   */
  std::regex ss = std::regex("^\\d+(\\.\\d+){0,1}$");
  std::regex mm_ss = std::regex("^\\d{2}:\\d{2}(\\.\\d+){0,1}$");
  std::regex hh_mm_ss = std::regex("^\\d{2}:\\d{2}:\\d{2}(\\.\\d+){0,1}$");
  std::regex ddd_hh_mm_ss = std::regex("^\\d{3}(:|\\s)\\d{2}:\\d{2}:\\d{2}(\\.\\d+){0,1}$");
  std::regex mm_dd_hh_mm_ss = std::regex("^\\d{2}(:|\\-|\\s)\\d{2}(:|\\s)\\d{2}:\\d{2}:\\d{2}(\\.\\d+){0,1}$");
  std::regex yyyy_ddd_hh_mm_ss = std::regex("^\\d{4}(:|\\-|\\s)\\d{3}(:|\\s)\\d{2}:\\d{2}:\\d{2}(\\.\\d+){0,1}$");
  std::regex yyyy_mm_dd_hh_mm_ss = std::regex("^\\d{4}(:|\\-|\\s)\\d{2}(:|\\-|\\s)\\d{2}(:|\\s)\\d{2}:\\d{2}:\\d{2}(\\.\\d+){0,1}$");

  char* str = timeString.data();
  if (std::regex_match(str, ss))
    return Format::SECONDS;

  if (std::regex_match(str, mm_ss))
    return Format::MM_SS;

  if (std::regex_match(str, hh_mm_ss))
    return Format::HH_MM_SS;

  if (std::regex_match(str, ddd_hh_mm_ss))
    return Format::DDD_HH_MM_SS;

  if (std::regex_match(str, mm_dd_hh_mm_ss))
    return Format::MM_DD_HH_MM_SS;

  if (std::regex_match(str, yyyy_ddd_hh_mm_ss))
    return Format::YYYY_DDD_HH_MM_SS;

  if (std::regex_match(str, yyyy_mm_dd_hh_mm_ss))
    return Format::YYYY_MM_DD_HH_MM_SS;

  return Format::INVALID;
}


tm TimeParser::SecondsToTm(double seconds, double& remainder)
{
  tm t;
  int days;
  int hours;
  int mins;

  long long sec = (long long)seconds;
  remainder = seconds - sec;

  // remove any years
  sec = sec % (60 * 60 * 24 * 365);

  days = sec / (60 * 60 * 24);
  sec -= days;

  hours = sec / (60 * 60);
  sec -= hours;

  mins = sec / 60;
  sec -= mins;

  t.tm_year = 0;
  t.tm_yday = days;
  t.tm_hour = hours;
  t.tm_min = mins;
  t.tm_sec = (int)sec;

  return t;
}