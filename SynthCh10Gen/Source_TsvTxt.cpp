#include "Source_TsvTxt.h"


ClSource_TsvTxt::ClSource_TsvTxt(ClSimState* pclSimState, std::string sPrefix)
  : ClSource_CsvTxt(pclSimState, sPrefix)
{
}

ClSource_TsvTxt::~ClSource_TsvTxt()
{
}

bool ClSource_TsvTxt::ParseLine(char* szLine, CSV_FIELDS& fields)
{
  // Tokenize line and store in fields

  char szTrimmedToken[100];
  int   iTokens;

  char* szToken = strtok(szLine, "\t");

  while (szToken != nullptr)
  {
    iTokens = sscanf(szToken, "%s", &szTrimmedToken);
    if (iTokens == 1)
      fields.push_back(szTrimmedToken);

    szToken = strtok(NULL, "\t");
  }

  return true;
}

bool ClSource_TsvTxt::ParseLine(char* szLine, CSV_FIELDS& labels, KEY_VAL_FIELDS& fieldMap)
{
  CSV_FIELDS fields;
  bool status = ParseLine(szLine, fields);

  if (!status || labels.size() != fields.size())
    return false;

  for (int i = 0; i < labels.size(); i++)
    fieldMap.insert(pair(labels[i], fields[i]));

  return true;
}
