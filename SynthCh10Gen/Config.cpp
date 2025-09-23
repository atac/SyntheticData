#include "Config.h"

Config::Config(string configPathname) {
  valid = true;

  Open(configPathname);

  if (Valid())
    ParseConfig();

  file->close();
  delete file;

  configTime = time(nullptr);
}

void Config::Open(string pathname) {
  file = new ifstream(pathname);
  if (!file->is_open()) {
    valid = false;
    return;
  }
}

void Config::ParseConfig() {
  try {
    config = json::parse(*file);

    valid = ConfigIsValid();

    if (!Valid())
      return;

    ParseGeneralInfo();
    ParseMappings();
    ParseChannels();
  }
  catch (json::exception e) {
    valid = false;
    printf("%s\n", e.what());
  }
}

bool Config::ConfigIsValid() {
  bool valid = true;

  try {
    json outDir = config["outputDirectory"];

    if (outDir.is_null() || !outDir.is_string())
      valid = false;

    if (valid) {
      valid = filesystem::exists(outDir.get<string>());
    }

    if (valid) {
      bool foundValidChannel = false;

      json chanList = config["channels"];

      if (!chanList.is_null() && chanList.is_array() && chanList.size() > 0) {
        for (auto c : chanList) {
          if (ChannelIsValid(c)) {
            foundValidChannel = true;
            break;
          }
        }
      }

      if (!foundValidChannel)
        valid = false;
    }

  }
  catch (json::exception e) {
    printf("Error parsing JSON: \n%s\n", e.what());
    valid = false;
  }

  return valid;
}

bool Config::ChannelIsValid(json channel) {
  auto t = channel.find("type");
  if (t == channel.end() 
    || !t->is_string()
    || GetChannelTypeFromString(t->get<string>()) == Ch10Channel::ChannelType::INVALID)
    return false;

  auto f = channel.find("format");
  if (f != channel.end() &&
    (!f->is_string() ||
      GetChannelDataFormatFromString(f->get<string>()) == Ch10Channel::ChannelDataFormat::INVALID
      )
    )
    return false;

  auto s = channel.find("source");
  if (s == channel.end() || !s->is_object())
    return false;

  if (!SourceIsValid(*s))
    return false;

  return true;
}

bool Config::SourceIsValid(json source) {
  auto n = source.find("pathname");
  if (n == source.end()
    || !n->is_string())
    return false;

  SourceFileType sft = GetSourceFileTypeFromString(n->get<string>());
  if (sft == SourceFileType::INVALID)
    return false;

  switch (sft)
  {
  case SourceFileType::SQLITE:
  {
    auto t = source.find("table");
    if (t == source.end()
      || !t->is_string())
      return false;
    break;
  }
  default:
    break;
  }

  if (!filesystem::exists(n->get<string>()))
    return false;

  return true;
}

void Config::ParseGeneralInfo() {

  // programName
  auto pn = config.find("programName");
  if (pn != config.end() && pn->is_string())
    programName = pn.value();
  else
    programName = GenerateProgramName();

  // outputDirectory
  outputDirectory = config["outputDirectory"];

  // outputPathname
  auto of = config.find("outputFilename");
  if (of != config.end() && of->is_string())
    outputFilename = of.value();
  else
    outputFilename = GenerateOutputFilename();

  // startTime
  auto st = config.find("startTime");
  if (st != config.end() && st->is_string())
    startTime = st.value();
  else
    startTime = "";

  // timeSourceChannel
  auto tsc = config.find("timeSource");
  if (tsc != config.end() && tsc->is_string()) {
    timeSourceChannel = tsc.value();
    transform(
      timeSourceChannel.begin(), 
      timeSourceChannel.end(),
      timeSourceChannel.begin(), 
      ::tolower);
  }
  else
    timeSourceChannel = "";
}

void Config::ParseMappings() {
  if (config.contains("mappings"))
  {
    json maps = config["mappings"];
    
    for (auto& [name, map] : maps.items()) {
      if (map.is_object())
        ParseMapping(name, map);
    }
  }
}

void Config::ParseMapping(string name, json& map) {
  ConfigMapping mapping;

  for (auto& [targetValue, inputValue] : map.items()) {
    if (inputValue.is_string()) {
      string inputString = inputValue.get<string>();

      if (!inputString.empty())
        mapping.insert(pair<string, string>(inputString, targetValue));
    }
  }

  if (!mapping.empty())
    mappings.insert(pair<string, ConfigMapping>(name, mapping));
}

void Config::ParseChannels() {
  json chanList = config["channels"];

  for (auto chan : chanList) {
    if (ChannelIsValid(chan))
      ParseChannel(chan);
  }

  GenerateUnspecifiedChannelIDs();
  GenerateUnspecifiedChannelNames();
}

void Config::ParseChannel(json channel) {
  Ch10Channel::ChannelType t = GetChannelTypeFromString(channel["type"].get<string>());

  int id;
  if (channel.contains("id") && channel["id"].is_number_unsigned())
    id = channel["id"].get<int>();
  else
    id = -1;

  ConfigChannel c(t, id);

  c.dataSource = ParseDataSource(channel["source"]);


  if (channel.contains("format") && channel["format"].is_string())
    c.format = GetChannelDataFormatFromString(channel["format"].get<string>());
  else
    c.format = Ch10Channel::ChannelDataFormat::UNFORMATTED;

  if (channel.contains("name") && channel["name"].is_string())
    c.name = channel["name"].get<string>();
  else
    c.name = "";

  if (channel.contains("pollRate"))
    c.pollRate = ParseRate(channel["pollRate"]);
  else
    c.pollRate = Rate(50, RateUnit::HERTZ);

  if (channel.contains("packetRate"))
    c.packetRate = ParseRate(channel["packetRate"]);
  else
    c.packetRate = Rate(10, RateUnit::HERTZ);

  CheckForTimeSource(c);

  channels.push_back(c);
}

ConfigDataSource Config::ParseDataSource(json source) {
  ConfigDataSource ds;

  ds.pathname = source["pathname"].get<string>();
  ds.type = GetSourceFileTypeFromString(ds.pathname);
   
  if (source.contains("mapping") && source["mapping"].is_string())
    ds.mapping = GetMappingByName(source["mapping"].get<string>());

  switch (ds.type) {

  case SourceFileType::SQLITE:
    if (source.contains("table") && source["table"].is_string())
      ds.properties.insert(pair("tableName", source["table"].get<string>()));
    break;

  default:
    break;
  }

  return ds;
}

Rate Config::ParseRate(json rate) {
  Rate r = Rate();

  auto value = rate.find("value");
  if (value != rate.end() && value->is_number_unsigned())
    r.value = value->get<unsigned int>();

  auto unit = rate.find("unit");
  if (unit != rate.end() && unit->is_string())
    r.units = GetRateUnitFromString(unit->get<string>());

  return r;
}

bool Config::Valid() {
  return valid;
}

Ch10Channel::ChannelType Config::GetChannelTypeFromString(string typeStr) {
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

Ch10Channel::ChannelDataFormat Config::GetChannelDataFormatFromString(string fmtStr) {
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

RateUnit Config::GetRateUnitFromString(string unitStr) {
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

SourceFileType Config::GetSourceFileTypeFromString(string pathname) {
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

ConfigMapping Config::GetMappingByName(string mapName) {
  auto m = mappings.find(mapName);
  if (m != mappings.end())
    return (m->second);
  
  return ConfigMapping();
}

string Config::GenerateProgramName() {
  return "Synthetic Chapter 10";
}

string Config::GenerateOutputFilename() {
  string prefix = ".\\synthetic_data_";
  string extension = ".ch10";

  struct tm* time = localtime(&configTime);
  char timestamp[20];
  strftime(timestamp, 20, "%Y%m%d_%H%M%S", time);

  return prefix + string(timestamp) + extension;
}

void Config::GenerateUnspecifiedChannelIDs() {
  set<int> ids;

  // get all specified ids for collision detection
  for (auto c : channels) {
    if (c.id > 1)
      ids.insert(c.id);
  }

  // generate unspecified ids checking for collision
  for (auto c = channels.begin(); c != channels.end(); c++) {
    if (c->id == -1) {

      do {
        c->id = GenerateChannelID();
      } while (ids.find(c->id) != ids.end());
    }
  }
}

int Config::GenerateChannelID() {
  static int nextID = 2;

  return nextID++;
}

void Config::GenerateUnspecifiedChannelNames() {
  for (auto c = channels.begin(); c != channels.end(); c++) {
    if (c->name == "")
      c->name = GenerateChannelName(c->id, c->type);
  }
}

string Config::GenerateChannelName(int channelID, Ch10Channel::ChannelType type) {
  string typestr = "PCM";

  if (type == Ch10Channel::ChannelType::MS1553)
    typestr = "1553";
  if (type == Ch10Channel::ChannelType::A429)
    typestr = "A429";
  if (type == Ch10Channel::ChannelType::VIDEO)
    typestr = "VIDEO";

  return typestr + "in" + to_string(channelID);
}

void Config::CheckForTimeSource(ConfigChannel& channel) {
  if (!timeSourceFound) {
    if (timeSourceChannel.size() == 0) {
      channel.timeSource = true;
      timeSourceFound = true;
    }
    else {
      string n = channel.name;
      transform(n.begin(), n.end(), n.begin(), ::tolower);
      if (n == timeSourceChannel) {
        channel.timeSource = true;
        timeSourceFound = true;
      }
    }
  }
}