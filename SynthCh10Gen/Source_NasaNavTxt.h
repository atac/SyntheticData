#pragma once

#include <string>
#include <vector>
#include <unordered_map>

// https://github.com/ben-strasser/fast-cpp-csv-parser
//#include "csv.h"

// https://github.com/rakeshgk/csv-parser
//#include "csv_parser.hpp"

#include "Source_Nav.h"
#include "Source_CsvTxt.h"
#include "SimState.h"

class ClSource_NasaNavTxt : 
        public ClSource_CsvTxt
    {
public:
    ClSource_NasaNavTxt(ClSimState * pclSimState, std::string sPrefix);
    ~ClSource_NasaNavTxt();

    // Class variables
protected:

public:

    // Methods
public:
    void Init();
//    bool ReadNextLine();
    bool UpdateSimState(double fSimElapsedTime);

    };

