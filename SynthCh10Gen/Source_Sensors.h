#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "SimState.h"

class ClSource_Sensors
    {
    public:
        ClSource_Sensors(ClSimState * pclSimState, std::string sPrefix);
        ~ClSource_Sensors();

public:
    // Class variables
    std::string                                 sPrefix;
    std::vector<std::string>                    DataLabel;
    ClSimState                                * pclSimState;

    // Methods
public:
    bool Open();
    void Close();
    bool Update();

    };

