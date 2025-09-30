#pragma once

#include <string>
#include <vector>
#include <set>
#include <fstream>
#include <ctime>

#include "GenerationControllerTypes.h"
#include "Logger.h"
#include "ConfigLib.h"
#include "ConfigValidator.h"

using namespace std;
using namespace GenerationConfig;



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

  Logger log;

private:
  time_t configTime;

  ifstream* file;
  json config;

  map<string, ConfigMapping> mappings;

  bool valid;

  string timeSourceChannel;
  bool timeSourceFound = false;

  void Open(string pathname);

  bool ChannelIsValid(json& channel);

  void ParseConfig();

  void ParseGeneralInfo();
  void ParseMappings();
  void ParseMapping(string name, json& map);
  void ParseChannels();
  void ParseChannel(json channel);
  ConfigDataSource ParseDataSource(json source);
  Rate ParseRate(json rate);

  ConfigMapping GetMappingByName(string mapName);

  string GenerateProgramName();
  string GenerateOutputFilename();
  void GenerateUnspecifiedChannelIDs();
  int GenerateChannelID();
  void GenerateUnspecifiedChannelNames();
  string GenerateChannelName(int channelID, Ch10Channel::ChannelType type);

  void CheckForTimeSource(ConfigChannel& channel);
};
