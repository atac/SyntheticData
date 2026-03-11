#include "Configuration.h"

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

    ConfigValidator& v = ConfigValidator::GetValidator();
    valid = v.Validate(config, logger);

    if (!Valid())
      return;

    ParseGeneralInfo();
    ParseDataSources();
    ParseMappings();
    ParseChannels();
  }
  catch (json::exception e) {
    valid = false;
    printf("%s\n", e.what());
  }
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
  if (st != config.end()) {
    if (st->is_string())
      startTime = st->get<string>();
    else if (st->is_number())
      startTime = to_string(st->get<double>());
  }
  else
    startTime = "";

  // timeBasisChannel
  auto tsc = config.find("timeBasis");
  if (tsc != config.end() && tsc->is_string()) {
    timeBasisChannel = tsc.value();
    transform(
      timeBasisChannel.begin(),
      timeBasisChannel.end(),
      timeBasisChannel.begin(),
      ::tolower);
  }
  else
    timeBasisChannel = "";
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

  CheckForTimeBasis(c);

  channels.push_back(c);
}

void Config::ParseDataSources() {
  if (config.contains("sources"))
  {
    json srcs = config["sources"];

    for (auto& [name, src] : srcs.items()) {
      if (src.is_object()) {
        ConfigDataSource cds = ParseDataSource(src);
        this->sources.insert(pair(name, cds));
      }
    }
  }
}

ConfigDataSource Config::ParseDataSource(json source) {
  ConfigDataSource ds;

  if (source.is_string()) {
    string srcName = source.get<string>();
    ds = GetDataSourceByName(srcName);
  }
  else {

    ds.pathname = source["pathname"].get<string>();
    ds.type = GetSourceFileTypeFromString(ds.pathname);

    if (source.contains("mapping") && source["mapping"].is_string())
      ds.mapping = GetMappingByName(source["mapping"].get<string>());

    if (source.contains("timeUnits") && source["timeUnits"].is_string())
      ds.timeUnits = TimeParser::GetTimeUnitFromString(source["timeUnits"].get<string>());

    if (source.contains("timeShift") && source["timeShift"].is_number())
      ds.timeShift = source["timeShift"].get<double>();

    switch (ds.type) {

    case SourceFileType::SQLITE:
      if (source.contains("table") && source["table"].is_string())
        ds.properties.insert(pair("tableName", source["table"].get<string>()));
      break;

    default:
      break;
    }
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


ConfigDataSource Config::GetDataSourceByName(string srcName) {
  auto s = sources.find(srcName);
  if (s != sources.end())
    return (s->second);

  return ConfigDataSource();
}

ConfigMapping Config::GetMappingByName(string mapName) {
  auto m = mappings.find(mapName);
  if (m != mappings.end())
    return (m->second);
  
  return ConfigMapping();
}

bool Config::ChannelIsValid(json& channel) {
  auto validProp = channel.find(ConfigValidator::validProperty);

  assert(validProp != channel.end());

  if (validProp->is_string())
    return validProp->get<string>() == "true";

  return false;
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

void Config::CheckForTimeBasis(ConfigChannel& channel) {
  if (!timeBasisFound) {
    if (timeBasisChannel.size() == 0) {
      channel.timeBasis = true;
      timeBasisFound = true;
    }
    else {
      string n = channel.name;
      transform(n.begin(), n.end(), n.begin(), ::tolower);
      if (n == timeBasisChannel) {
        channel.timeBasis = true;
        timeBasisFound = true;
      }
    }
  }
}