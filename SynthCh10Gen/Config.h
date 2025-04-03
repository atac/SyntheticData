#pragma once

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <ctime>

#include "GenerationControllerTypes.h"

#include "nlohmann/json.hpp"

using namespace std;
using namespace nlohmann;


struct ConfigDataSource;
struct ConfigChannel;
typedef map<string, string> ConfigMapping;


class Config
{
public:
  Config(string configPathname);

  bool Valid();

  string programName;
  string outputDirectory;
  string outputFilename;
  string startTime;

  vector<ConfigChannel> channels;

private:
  time_t configTime;

  ifstream* file;
  json config;

  map<string, ConfigMapping> mappings;

  bool valid;

  string timeSourceChannel;
  bool timeSourceFound = false;

  void Open(string pathname);

  bool ConfigIsValid();
  bool ChannelIsValid(json channel);
  bool SourceIsValid(json source);

  void ParseConfig();

  void ParseGeneralInfo();
  void ParseMappings();
  void ParseMapping(string name, json& map);
  void ParseChannels();
  void ParseChannel(json channel);
  ConfigDataSource ParseDataSource(json source);
  Rate ParseRate(json rate);

  Ch10Channel::ChannelType GetChannelTypeFromString(string typeStr);
  Ch10Channel::ChannelDataFormat GetChannelDataFormatFromString(string formatStr);
  RateUnit GetRateUnitFromString(string unitStr);
  SourceFileType GetSourceFileTypeFromString(string pathname);
  ConfigMapping GetMappingByName(string mapName);

  string GenerateProgramName();
  string GenerateOutputFilename();
  void GenerateUnspecifiedChannelIDs();
  int GenerateChannelID();
  void GenerateUnspecifiedChannelNames();
  string GenerateChannelName(int channelID, Ch10Channel::ChannelType type);

  void CheckForTimeSource(ConfigChannel channel);
};

struct ConfigDataSource
{
  string pathname;
  SourceFileType type;
  ConfigMapping mapping;
  map<string, string> properties;
};

struct ConfigChannel 
{
  ConfigChannel(Ch10Channel::ChannelType type, int id) : type(type), id(id) { };

  bool timeSource;
  int id;
  Ch10Channel::ChannelType type;
  Ch10Channel::ChannelDataFormat format;
  string name;
  ConfigDataSource dataSource;
  Rate pollRate;
  Rate packetRate;
};