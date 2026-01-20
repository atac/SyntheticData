#include "Source_TsvTxt.h"


ClSource_TsvTxt::ClSource_TsvTxt(ClSimState* pclSimState, std::string sPrefix)
  : ClSource_CsvTxt(pclSimState, sPrefix)
{
}

ClSource_TsvTxt::~ClSource_TsvTxt()
{
}

bool ClSource_TsvTxt::ParseLine(char* szLine, StringList& values)
{
  // Tokenize line and store in fields

  char szTrimmedToken[100];
  int   iTokens;

  char* szToken = strtok(szLine, "\t");

  while (szToken != nullptr)
  {
    iTokens = sscanf(szToken, "%s", &szTrimmedToken);
    if (iTokens == 1)
      values.push_back(szTrimmedToken);

    szToken = strtok(NULL, "\t");
  }

  return true;
}

bool ClSource_TsvTxt::ParseLineToMap(char* szLine, KEY_VAL_FIELDS& valueMap)
{
  StringList values;
  bool status = ParseLine(szLine, values);

  if (!status || fields.size() != values.size())
    return false;

  for (int i = 0; i < values.size(); i++) {
    valueMap.insert_or_assign(fields[i].getID(), values[i]);
  }

  return true;
}
