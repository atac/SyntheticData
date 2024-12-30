#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <ctime>

#include "GenerationControllerTypes.h"

#include "nlohmann/json.hpp"

using namespace std;
using namespace nlohmann;


struct ConfigChannel;


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

  bool valid;

  string timeSourceChannel;
  bool timeSourceFound = false;

  void Open(string pathname);

  bool ConfigIsValid();
  bool ChannelIsValid(json channel);

  void ParseConfig();

  void ParseGeneralInfo();
  void ParseChannels();
  void ParseChannel(json channel);
  Rate ParseRate(json rate);

  Ch10Channel::ChannelType GetChannelTypeFromString(string typeStr);
  RateUnit GetRateUnitFromString(string unitStr);

  string GenerateProgramName();
  string GenerateOutputFilename();
  int GenerateChannelID();
  string GenerateChannelName(int channelID, Ch10Channel::ChannelType type);

  void CheckForTimeSource(ConfigChannel channel);
};

struct ConfigChannel 
{
  ConfigChannel(Ch10Channel::ChannelType type, int id) : type(type), id(id) { };

  bool timeSource;
  int id;
  Ch10Channel::ChannelType type;
  string name;
  string sourcePathname;
  Rate pollRate;
  Rate packetRate;
};