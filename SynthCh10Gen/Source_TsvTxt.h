#pragma once
#include "Source_CsvTxt.h"

class ClSource_TsvTxt :
    public ClSource_CsvTxt
{
public:
  ClSource_TsvTxt(ClSimState* pclSimState, std::string sPrefix);
  ~ClSource_TsvTxt();

private:
  bool ParseLine(char* szLine, StringList& values) override;
  bool ParseLineToMap(char* szLine, KEY_VAL_FIELDS& valueMap) override;
};

