#pragma once

#include <string>
#include <regex>


class TimeParser
{
public:
  enum class Format {
    INVALID,
    NANOSECONDS,
    MICROSECONDS,
    MILLISECONDS,
    SECONDS,
    MM_SS,
    HH_MM_SS,
    DDD_HH_MM_SS,
    MM_DD_HH_MM_SS,
    YYYY_DDD_HH_MM_SS,
    YYYY_MM_DD_HH_MM_SS
  };

  enum class Units {
    INVALID,
    SECONDS,
    DAYS
  };

public:
  TimeParser();
  ~TimeParser();

  bool Valid();

  void Init(std::string timeString, Units units = Units::SECONDS);
  void Init(Format format, Units units);

  bool Parse(std::string timeString, double& timeSeconds);
  Format GetFormat();


  static TimeParser::Units GetTimeUnitFromString(std::string unitStr);

private:
  Format format;
  Units units;

  bool (TimeParser::*DoParse)(std::string timeString, double& time);

  void SetParseMethod();
  bool Parse_INVALID(std::string sTime, double& time);
  bool Parse_Double(std::string sTime, double& time);
  bool Parse_MM_SS(std::string sTime, double& time);
  bool Parse_HH_MM_SS(std::string sTime, double& time);
  bool Parse_DDD_HH_MM_SS(std::string sTime, double& time);
  bool Parse_MM_DD_HH_MM_SS(std::string sTime, double& time);
  bool Parse_YYYY_DDD_HH_MM_SS(std::string sTime, double& time);
  bool Parse_YYYY_MM_DD_HH_MM_SS(std::string sTime, double& time);

  static Format DetermineFormat(std::string timeString);
  static tm SecondsToTm(double seconds, double& remainder);
};
