/* Source_CsvTxt.h - Derived class for all CSV input simulation state input operations.

This class is used as the basis for reading simulation state from a CSV file. This class
can be used as-is but some important assumptions are made when using this class.

1) All the input data can be represented with floating point numbers.
2) All the data headers names are OK as-is and don't need any fixup.

*/

#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// https://github.com/ben-strasser/fast-cpp-csv-parser
//#include "csv.h"

#include "Source_Nav.h"
#include "SimState.h"

class ClSource_CsvTxt : public ClSource_Nav
{
public:
  ClSource_CsvTxt(ClSimState* pclSimState, std::string sPrefix);
  ~ClSource_CsvTxt();

  // Class variables
protected:
  FILE*           hCsvInput;
  KEY_VAL_FIELDS  CsvMap;

private:
  CSV_Parser      CsvParser;

public:

  // Methods
public:
  virtual bool    Open(std::string sFilename);
  virtual void    Close();
  virtual bool    ReadNextLine();
  virtual bool    UpdateSimState(double fSimElapsedTime);
  virtual void    SetMapping(ConfigMapping map);

protected:
  void Init();

  bool ReadLineToBuffer(char* buf, size_t bufLen, fpos_t* lastPosition = nullptr);
  bool HasNumericData(CSV_FIELDS& values);

  virtual void ApplyMapping();
  virtual void ApplyPrefix();

  virtual bool ParseLine(char* szLine, CSV_FIELDS& fields);
  virtual bool ParseLine(char* szLine, CSV_FIELDS& labels, KEY_VAL_FIELDS& fieldMap);
};

