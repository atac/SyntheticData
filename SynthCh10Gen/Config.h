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
enum class ConfigChannelType;


class Config
{
public:
  Config(string configPathname);

  bool Valid();

  string programName;
  string outputPathname;
  string startTime;

  vector<ConfigChannel> channels;

private:
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

  ConfigChannelType GetChannelTypeFromString(string typeStr);
  RateUnit GetRateUnitFromString(string unitStr);

  string GenerateProgramName();
  string GenerateOutputPathname();
  int GenerateChannelID();
  string GenerateChannelName(int channelID, ConfigChannelType type);

  void CheckForTimeSource(ConfigChannel channel);
};

struct ConfigChannel 
{
  ConfigChannel(ConfigChannelType type, int id) : type(type), id(id) { };

  bool timeSource;
  int id;
  ConfigChannelType type;
  string name;
  string sourcePathname;
  Rate pollRate;
  Rate packetRate;
};

enum class ConfigChannelType {
  INVALID,
  PCM,
  ARINC_429,
  MIL_STD_1553,
  VIDEO
};