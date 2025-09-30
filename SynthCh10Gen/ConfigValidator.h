#pragma once

#include "ConfigLib.h"

#include "nlohmann/json.hpp"


using namespace std;
using namespace nlohmann;
using namespace GenerationConfig;


class ConfigValidator
{
private:
  ConfigValidator() {};

  ConfigValidator(const ConfigValidator&) = delete;
  ConfigValidator& operator=(const ConfigValidator&) = delete;

public:
  bool Validate(json& config);

private:
  bool ConfigIsValid(json& config);
  bool GeneralInfoIsValid(json& config);
  bool ChannelIsValid(json& channel);
  bool SourceIsValid(json& source);


public:
  inline static ConfigValidator& GetValidator() {
    static ConfigValidator validator;
    return validator;
  }

  static inline const string validProperty = "validationResult";
};

