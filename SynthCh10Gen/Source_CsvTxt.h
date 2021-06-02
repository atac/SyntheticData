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

// https://github.com/rakeshgk/csv-parser
#include "csv_parser.hpp"

#include "Source_Nav.h"
#include "SimState.h"

class ClSource_CsvTxt  : public ClSource_Nav
    {
    public:
        ClSource_CsvTxt(ClSimState * pclSimState, std::string sPrefix);
        ~ClSource_CsvTxt();

    // Class variables
protected:
    FILE                                      * hCsvInput;
    CSV_Parser                                  CsvParser;
    CSV_FIELDS                                  CsvDataLabels;
    KEY_VAL_FIELDS                              CsvMap;

public:

    // Methods
public:
    virtual bool    Open(std::string sFilename);
    virtual void    Init();
    virtual void    Close();
    virtual bool    ReadNextLine();
    virtual bool    UpdateSimState(double fSimElapsedTime);
    virtual bool    ConvertTime(std::string sTime, double *fDecodedTime);

    };

