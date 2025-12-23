#pragma once

#include "ConfigLib.h"
#include "Logger.h"

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
    vector<string> validMappingNames;
    vector<string> validSingleDefSourceNames;
    string timeBasisChannelName;
    bool foundTimeBasisChannel;
    bool foundValidChannel;
    bool foundInvalidChannel;
  } state;

public:
  bool Validate(json& config, Logger& logger);

private:
  bool ConfigIsValid(json& config);
  bool GeneralInfoIsValid(json& config);
  bool MappingsAreValid(json& config);
  bool MappingIsValid(json& mapping);
  bool ChannelsAreValid(json& config);
  bool ChannelIsValid(json& channel);
  bool SingleDefinitionSourcesAreValid(json& channel);
  bool SourceIsValid(json& source);
  bool RateIsValid(json& rate);

  void LogError(string msg);
  void LogInfo(string msg);


public:
  inline static ConfigValidator& GetValidator() {
    static ConfigValidator validator;
    return validator;
  }

  static inline const string validProperty = "validationResult";
};

