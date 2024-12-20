#include "Config.h"

Config::Config(string configPathname) {
  valid = true;

  Open(configPathname);

  if (Valid())
    ParseConfig();

  file->close();
  delete file;
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

    if (!ConfigIsValid())
      return;

    ParseGeneralInfo();
    ParseChannels();
  }
  catch (json::exception e) {
    valid = false;
    printf("%s\n", e.what());
  }
}

bool Config::ConfigIsValid() {
  try {
    bool valid = false;

    json chanList = config["channels"];

    if (!chanList.is_null() && chanList.is_array() && chanList.size() > 0) {
      for (auto c : chanList) {
        if (ChannelIsValid(c)) {
          valid = true;
          break;
        }
      }
    }
  }
  catch (json::exception e) {
    printf("Error parsing JSON: \n%s\n", e.what());
  }

  return valid;
}

bool Config::ChannelIsValid(json channel) {
  auto t = channel.find("type");
  if (t == channel.end() 
    || !t->is_string()
    || GetChannelTypeFromString(t->get<string>()) == Ch10Channel::ChannelType::INVALID)
    return false;

  auto sf = channel.find("sourceFile");
  if (sf == channel.end() || !sf->is_string())
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

  // outputPathname
  auto of = config.find("outputFile");
  if (of != config.end() && of->is_string())
    outputPathname = of.value();
  else
    outputPathname = GenerateOutputPathname();

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

void Config::ParseChannels() {
  json chanList = config["channels"];

  for (auto chan : chanList) {
    if (ChannelIsValid(chan))
      ParseChannel(chan);
  }
}

void Config::ParseChannel(json channel) {
  Ch10Channel::ChannelType t = GetChannelTypeFromString(channel["type"].get<string>());

  int id;
  if (channel.contains("id") && channel["id"].is_number_unsigned())
    id = channel["id"].get<int>();
  else
    id = GenerateChannelID();

  ConfigChannel c(t, id);

  c.sourcePathname = channel["sourceFile"];

  if (channel.contains("name") && channel["name"].is_string())
    c.name = channel["name"].get<string>();
  else
    c.name = GenerateChannelName(id, t);

  if (channel.contains("pollRate"))
    c.pollRate = ParseRate(channel["pollRate"]);

  if (channel.contains("packetRate"))
    c.packetRate = ParseRate(channel["packetRate"]);

  CheckForTimeSource(c);

  channels.push_back(c);
}

Rate Config::ParseRate(json rate) {
  Rate r = Rate();

  auto value = rate.find("value");
  if (value == rate.end() || !value->is_number_unsigned())
    r.value = value->get<unsigned int>();

  auto unit = rate.find("unit");
  if (unit == rate.end() || !unit->is_string())
    r.units = GetRateUnitFromString(unit->get<string>());

  return r;
}

bool Config::Valid() {
  return valid;
}

Ch10Channel::ChannelType Config::GetChannelTypeFromString(string typeStr) {
  Ch10Channel::ChannelType t = Ch10Channel::ChannelType::INVALID;

  transform(typeStr.begin(), typeStr.end(), typeStr.begin(), ::tolower);

  if (typeStr == "pcm")
    t = Ch10Channel::ChannelType::PCM;
  else if (typeStr == "a429" || typeStr == "arinc-429" || typeStr == "arinc_429")
    t = Ch10Channel::ChannelType::A429;
  else if (typeStr == "1553" || typeStr == "mil-std-1553" || typeStr == "mil_std_1553" || typeStr == "ms1553")
    t = Ch10Channel::ChannelType::MS1553;
  else if (typeStr == "video" || typeStr == "vid")
    t = Ch10Channel::ChannelType::VIDEO;

  return t;
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

string Config::GenerateProgramName() {
  time_t t = time(NULL);
  return "Synthetic Data Generated " + string(ctime(&t));
}

string Config::GenerateOutputPathname() {
  string prefix = ".\\synthetic_data_";
  string extension = ".ch10";

  time_t t = time(NULL);
  struct tm* time = localtime(&t);
  char timestamp[14];
  strftime(timestamp, 14, "%Y%m%d%R%M%S", time);

  return prefix + string(timestamp) + extension;
}

int Config::GenerateChannelID() {
  static int nextID = 2;

  return nextID++;
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

void Config::CheckForTimeSource(ConfigChannel channel) {
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