#pragma once

#include <algorithm>
#include <map>

#include "GenerationControllerTypes.h"


using namespace std;

namespace GenerationConfig {


  typedef map<string, string> ConfigMapping;

  struct ConfigDataSource
  {
    string pathname;
    SourceFileType type;
    ConfigMapping mapping;
    map<string, string> properties;
  };

  struct ConfigChannel
  {
    ConfigChannel(Ch10Channel::ChannelType type, int id) : type(type), id(id)
    {
      timeSource = false;
      format = Ch10Channel::ChannelDataFormat::INVALID;
    };

    bool timeSource;
    int id;
    Ch10Channel::ChannelType type;
    Ch10Channel::ChannelDataFormat format;
    string name;
    ConfigDataSource dataSource;
    Rate pollRate;
    Rate packetRate;
  };


  static Ch10Channel::ChannelType GetChannelTypeFromString(string typeStr) {
    Ch10Channel::ChannelType t = Ch10Channel::ChannelType::INVALID;

    transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

    if (typeStr == "pcmin" || typeStr == "pcm")
      t = Ch10Channel::ChannelType::PCM;
    else if (typeStr == "429in" || typeStr == "a429" || typeStr == "arinc429" || typeStr == "arinc-429" || typeStr == "arinc_429")
      t = Ch10Channel::ChannelType::A429;
    else if (typeStr == "1553in" || typeStr == "1553" || typeStr == "mil-std-1553" || typeStr == "mil_std_1553" || typeStr == "ms1553")
      t = Ch10Channel::ChannelType::MS1553;
    else if (typeStr == "vidin" || typeStr == "video" || typeStr == "vid")
      t = Ch10Channel::ChannelType::VIDEO;

    return t;
  }

  static Ch10Channel::ChannelDataFormat GetChannelDataFormatFromString(string fmtStr) {
    Ch10Channel::ChannelDataFormat f = Ch10Channel::ChannelDataFormat::INVALID;

    transform(fmtStr.begin(), fmtStr.end(), fmtStr.begin(), ::tolower);

    if (fmtStr == "unformatted")
      f = Ch10Channel::ChannelDataFormat::UNFORMATTED;
    else if (fmtStr == "custom")
      f = Ch10Channel::ChannelDataFormat::CUSTOM;
    else if (fmtStr == "synthformat1" || fmtStr == "synthfmt1")
      f = Ch10Channel::ChannelDataFormat::SYNTHFORMAT1;

    return f;
  }

  static RateUnit GetRateUnitFromString(string unitStr) {
    RateUnit u = RateUnit::TIME_MS;

    transform(unitStr.begin(), unitStr.end(), unitStr.begin(), ::tolower);

    if (unitStr == "s" || unitStr == "sec" || unitStr == "seconds")
      u = RateUnit::TIME_SEC;
    else if (unitStr == "us" || unitStr == "micro" || unitStr == "microseconds")
      u = RateUnit::TIME_US;
    else if (unitStr == "rtc")
      u = RateUnit::TIME_RTC;
    else if (unitStr == "ns" || unitStr == "nano" || unitStr == "nanoseconds")
      u = RateUnit::TIME_NS;
    else if (unitStr == "hz" || unitStr == "hertz" || unitStr == "frequency")
      u = RateUnit::HERTZ;

    return u;
  }

  static SourceFileType GetSourceFileTypeFromString(string pathname) {
    size_t dotIndex = pathname.find_last_of('.');
    string ext = pathname.substr(dotIndex + 1);

    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    SourceFileType sft = SourceFileType::INVALID;

    if (ext == "csv")
      sft = SourceFileType::CSV;
    else if (ext == "sql")
      sft = SourceFileType::SQLITE;
    else if (ext == "txt")
      sft = SourceFileType::TSV;

    return sft;
  }


}