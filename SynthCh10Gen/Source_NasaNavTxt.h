#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// https://github.com/ben-strasser/fast-cpp-csv-parser
//#include "csv.h"

// https://github.com/rakeshgk/csv-parser
#include "csv_parser.hpp"

#include "SimState.h"

#define BLUEMAX_ASSOC

class ClSource_NasaNavTxt
    {
    public:
        ClSource_NasaNavTxt(ClSimState * pclSimState, std::string sPrefix);
        ~ClSource_NasaNavTxt();

    // Class variables
protected:
    FILE                                      * hNasaInput;
    CSV_Parser                                  CsvParser;
    CSV_FIELDS                                  CsvDataLabels;
    double                                      fStartTime;

public:
    std::string                                 sPrefix;
//    std::vector<std::string>                    SqlColumnNames;
    ClSimState                                * pclSimState;

    // Methods
public:
    bool Open(std::string sFilename);
    void Close();
    bool ReadNextLine();

    };

