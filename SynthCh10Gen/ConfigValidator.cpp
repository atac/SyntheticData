#include "ConfigValidator.h"


bool ConfigValidator::Validate(json& config, Logger& logger) {
  state.logger = &logger;
  state.timeSourceChannelName = "";
  state.foundTimeSourceChannel = false;
  state.foundValidChannel = false;
  state.foundInvalidChannel = false;

  bool valid = true;

  try {
    valid = ConfigIsValid(config);
  }
  catch (json::exception e) {
    printf("Error parsing JSON: \n%s\n", e.what());
    valid = false;
  }

  state.logger = nullptr;

  return valid;
}

bool ConfigValidator::ConfigIsValid(json& config)
{
  bool valid = true;

  if (!GeneralInfoIsValid(config)) {
    LogError("Error found in the general info configuration");
    valid = false;
  }

  if (!SingleDefinitionSourcesAreValid(config)) {
    LogError("Error found in one or more source configurations");
    valid = false;
  }

  if (!ChannelsAreValid(config)) {
    LogError("Error found in one or more channel configurations");
    valid = false;
  }

  if (valid)
    LogInfo("Validation successful");

  return valid;
}

bool ConfigValidator::GeneralInfoIsValid(json& config) {
  bool valid = true;

  json outDir = config["outputDirectory"];
  if (outDir.is_null()) {
    LogError("'outputDirectory' property not found");
    valid = false;
  }
  else if (!outDir.is_string()) {
    LogError("'outputDirectory' property value is not a string");
    valid = false;
  }
  else if (!filesystem::exists(outDir.get<string>())) {
    LogError("Output directory does not exist on the filesystem");
    valid = false;
  }

json timeSource = config["timeSource"];
if (!timeSource.is_null() && timeSource.is_string()) {
  state.timeSourceChannelName = timeSource.get<string>();
  transform(
    state.timeSourceChannelName.begin(),
    state.timeSourceChannelName.end(),
    state.timeSourceChannelName.begin(),
    ::tolower
  );
}

return valid;
}


bool ConfigValidator::SingleDefinitionSourcesAreValid(json& config) {
  bool valid = true;

  json& srcList = config["sources"];
  if (!srcList.is_null()) {
    for (auto [name, src] : srcList.items()) {
      if (!SourceIsValid(src)) {
        LogError("Single-definition source " + name + " is invalid");
        valid = false;
      }
      else {
        string lcname = name;
        transform(lcname.begin(), lcname.end(), lcname.begin(), ::tolower);
        state.validSingleDefSourceNames.push_back(lcname);
      }
    }
  }

  return valid;
}

bool ConfigValidator::ChannelsAreValid(json& config) {
  bool valid = true;

  json& chanList = config["channels"];

  if (chanList.is_null()) {
    LogError("Channels array not found");
    valid = false;
  }
  else if (!chanList.is_array()) {
    LogError("'channels' property value is not an array");
    valid = false;
  }
  else if (chanList.size() <= 0) {
    LogError("No channels found");
    valid = false;
  }
  else {
    for (auto& c : chanList) {
      if (ChannelIsValid(c))
        state.foundValidChannel = true;
      else
        state.foundInvalidChannel = true;
    }

    if (!state.foundValidChannel) {
      LogError("No valid channels found");
      valid = false;
    }
    else {
      if (state.foundInvalidChannel) {
        LogError("One or more channels are not valid");
        valid = false;
      }

      if (!state.foundTimeSourceChannel) {
        LogError("No valid channel was found that matches the time source designation");
        valid = false;
      }
    }
  }

  return valid;
}

bool ConfigValidator::ChannelIsValid(json& channel) {
  bool valid = true;

  string name = "";
  string nameLC = "";
  json n = channel["name"];
  if (!n.is_null() && n.is_string()) {
    name = n.get<string>();
    nameLC = name;
    transform(nameLC.begin(), nameLC.end(), nameLC.begin(), ::tolower);
    name = "(" + n.get<string>() + ")";
  }

  auto t = channel.find("type");
  if (t == channel.end()) {
    LogError("Channel " + name + " 'type' property not found");
    valid = false;
  }
  else if (!t->is_string()) {
    LogError("Channel " + name + " 'type' property is not a string");
    valid = false;
  }
  else if (GetChannelTypeFromString(t->get<string>()) == Ch10Channel::ChannelType::INVALID) {
    LogError("Channel " + name + " type invalid");
    valid = false;
  }

  auto f = channel.find("format");
  if (f != channel.end()) {
    if (!f->is_string()) {
      LogError("Channel " + name + " 'format' property is not a string");
      valid = false;
    }
    else if (GetChannelDataFormatFromString(f->get<string>()) == Ch10Channel::ChannelDataFormat::INVALID) {
      LogError("Channel " + name + " format invalid");
      valid = false;
    }
  }

  json& s = channel["source"];
  if (s.is_null()) {
    LogError("Channel " + name + " 'source' property not found");
    valid = false;
  }
  else if (s.is_string()) { // check for single-definition source
    string srcName = s.get<string>();
    transform(srcName.begin(), srcName.end(), srcName.begin(), ::tolower);

    auto iter = std::find(state.validSingleDefSourceNames.begin(), state.validSingleDefSourceNames.end(), srcName);
    if (iter == state.validSingleDefSourceNames.end()) {
      LogError("Channel " + name + " referenced source is not a valid single-definition source");
      valid = false;
    }
  }
  else if (!s.is_object()) {
    LogError("Channel " + name + " 'source' property is not a valid type");
    valid = false;
  }
  else {
    if (!SourceIsValid(s)) {
      LogError("Channel " + name + " source is not valid");
      valid = false;
    }
  }

  if (valid) {
    channel[validProperty] = "true";

    if (!state.foundTimeSourceChannel) {
      if (state.timeSourceChannelName.size() == 0) // then first valid channel is time source
        state.foundTimeSourceChannel = true;
      else {
        if (state.timeSourceChannelName == nameLC)
          state.foundTimeSourceChannel = true;
      }
    }
  }
  else
    channel[validProperty] = "false";

  return true;
}

bool ConfigValidator::SourceIsValid(json& source) {
  bool valid = true;

  auto n = source.find("pathname");
  if (n == source.end()) {
    LogError("Source 'pathname' property not found");
    valid = false;
  }
  else if (!n->is_string()) {
    LogError("Source 'pathname' property is not a string");
    valid = false;
  }
  else {
    SourceFileType sft = GetSourceFileTypeFromString(n->get<string>());
    if (sft == SourceFileType::INVALID) {
      LogError("Source file type is invalid");
      valid = false;
    }
    else {
      switch (sft)
      {
      case SourceFileType::SQLITE:
      {
        auto t = source.find("table");
        if (t == source.end()) {
          LogError("Source file type is SQLite DB, but no 'table' property was defined");
          valid = false;
        }
        else if (!t->is_string()) {
          LogError("Source file type is SQLite DB, but 'table' property is not a string");
          valid = false;
        }
        break;
      }
      default:
        break;
      }

      if (!filesystem::exists(n->get<string>())) {
        LogError("Source file does not exist on the filesystem: " + n->get<string>());
        valid = false;
      }
    }
  }

  if (valid)
    source[validProperty] = "true";
  else
    source[validProperty] = "false";

  return valid;
}



void ConfigValidator::LogError(string msg) {
  state.logger->Add(CONFIG_ERROR, msg);
}

void ConfigValidator::LogInfo(string msg) {
  state.logger->Add(CONFIG_INFO, msg);
}