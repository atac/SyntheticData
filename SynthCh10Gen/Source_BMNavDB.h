#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "sqlite3.h"

#include "Source_Nav.h"
#include "SimState.h"

#define BLUEMAX_ASSOC

#pragma message("WARNING - Source_BMNavDB has been restructured to make it compile but still has serious flaws that need to be fixed.")

class ClSource_BMNavDB : 
        public ClSource_Nav
    {
    public:
        ClSource_BMNavDB(ClSimState * pclSimState, std::string sPrefix);
        ~ClSource_BMNavDB();

public:
    // Class variables
    int                         iStatus;
    sqlite3                   * pDB;
    std::string                 sSQL;
    sqlite3_stmt              * pSqlStmt;

    std::vector<std::string>    asColLabel;

    // Methods
public:
    bool    Open(std::string sFilename);
    void    Close();
    bool    ReadNextLine();
    bool    UpdateSimState(double fSimElapsedTime);

    };

