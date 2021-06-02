#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "Source_Nav.h"
#include "SimState.h"

#define BLUEMAX_ASSOC

#pragma message("WARNING - Source_BMNavTxt has been restructured to make it compile but still has serious flaws that need to be fixed.")

class ClSource_BMNavTxt : 
        public ClSource_Nav
    {
    public:
        ClSource_BMNavTxt(ClSimState * pclSimState, std::string sPrefix);
        ~ClSource_BMNavTxt();

public:
    // Class variables
    FILE                                      * hBMInput;
    std::vector<std::string>                    DataLabel;
    char                                        szLine[2000];

    // Methods
public:
    bool    Open(std::string sFilename);
    void    Close();
    bool    ReadNextLine();
    bool    UpdateSimState(double fSimElapsedTime);

    };

