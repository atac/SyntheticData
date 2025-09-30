#include "ConfigValidator.h"


bool ConfigValidator::Validate(json& config) {
  try {
    ConfigIsValid(config);
  }
  catch (json::exception e) {
    printf("Error parsing JSON: \n%s\n", e.what());
    return false;
  }

  return true;
}

bool ConfigValidator::ConfigIsValid(json& config) {
  bool valid = true;

  json outDir = config["outputDirectory"];

  if (outDir.is_null() || !outDir.is_string())
    valid = false;

  if (valid) {
    valid = filesystem::exists(outDir.get<string>());
  }

  if (valid) {
    bool foundValidChannel = false;

    json& chanList = config["channels"];

    if (!chanList.is_null() && chanList.is_array() && chanList.size() > 0) {
      for (auto& c : chanList) {
        if (ChannelIsValid(c)) {
          foundValidChannel = true;
          c[validProperty] = "true";
        }
        else
          c[validProperty] = "false";
      }
    }

    if (!foundValidChannel)
      valid = false;
  }

  return valid;
}

bool ConfigValidator::ChannelIsValid(json& channel) {
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

  if (!SourceIsValid(*s)) {
    (*s)[validProperty] = "false";
    return false;
  }

  (*s)[validProperty] = "true";
  return true;
}

bool ConfigValidator::SourceIsValid(json& source) {
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
