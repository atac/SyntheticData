#pragma once

#include "ConfigLib.h"

#include "nlohmann/json.hpp"


using namespace std;
using namespace nlohmann;
using namespace GenerationConfig;


class ConfigValidator
{
private:
  ConfigValidator() = default;

  ConfigValidator(const ConfigValidator&) = delete;
  ConfigValidator& ConfigValidator::operator=(const ConfigValidator&) = delete;

  struct ValidatorState {
    Logger* logger;
    string timeSourceChannelName;
    bool foundTimeSourceChannel;
    bool foundValidChannel;
    bool foundInvalidChannel;
  } state;

public:
  bool Validate(json& config, Logger& logger);

private:
  bool ConfigIsValid(json& config);
  bool GeneralInfoIsValid(json& config);
  bool ChannelsAreValid(json& config);
  bool ChannelIsValid(json& channel);
  bool SourceIsValid(json& source);

  void LogError(string msg);
  void LogInfo(string msg);


public:
  inline static ConfigValidator& GetValidator() {
    static ConfigValidator validator;
    return validator;
  }

  static inline const string validProperty = "validationResult";
};

