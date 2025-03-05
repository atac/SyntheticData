#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Source_Nav.h"
#include "SimState.h"

// https://github.com/rakeshgk/csv-parser
#include "csv_parser.hpp"

#define BLUEMAX_ASSOC

#pragma message("WARNING - Source_BMNavTxt has been restructured to make it compile but still has serious flaws that need to be fixed.")

class ClSource_BMNavTxt :
  public ClSource_Nav
{
public:
  ClSource_BMNavTxt(ClSimState* pclSimState, std::string sPrefix, int aircraftIndex = 0);
  ~ClSource_BMNavTxt();

public:
  // Class variables
  CSV_FIELDS                                  CsvDataLabels;
  KEY_VAL_FIELDS                              CsvMap;
  FILE* hBMInput;
  char                                        szLine[2000];
  int aircraftIndex;

  // Methods
public:
  bool    Open(std::string sFilename);
  void    Close();
  bool    ReadNextLine();
  bool    UpdateSimState(double fSimElapsedTime);

};

