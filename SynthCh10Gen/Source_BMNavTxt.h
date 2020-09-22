#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "SimState.h"

#define BLUEMAX_ASSOC

class ClSource_BMNavTxt
    {
    public:
        ClSource_BMNavTxt(ClSimState * pclSimState, std::string sPrefix);
        ~ClSource_BMNavTxt();

public:
    // Class variables
    FILE                                      * hBMInput;
    std::string                                 sPrefix;
    std::vector<std::string>                    DataLabel;
    //std::unordered_map<std::string, double>   * pSimState;
    ClSimState                                * pclSimState;

    // Methods
public:
    bool Open(std::string sFilename);
    void Close();
    bool ReadNextLine();

    };

