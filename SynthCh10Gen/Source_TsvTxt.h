#pragma once
#include "Source_CsvTxt.h"

class ClSource_TsvTxt :
    public ClSource_CsvTxt
{
public:
  ClSource_TsvTxt(ClSimState* pclSimState, std::string sPrefix);
  ~ClSource_TsvTxt();

private:
  bool ParseLine(char* szLine, CSV_FIELDS& fields) override;
  bool ParseLine(char* szLine, CSV_FIELDS& labels, KEY_VAL_FIELDS& fieldMap) override;
};

