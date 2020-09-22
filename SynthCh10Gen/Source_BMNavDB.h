#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "sqlite3.h"

#include "SimState.h"

#define BLUEMAX_ASSOC

class ClSource_BMNavDB
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

    std::string                 sPrefix;
    std::vector<std::string>    asColLabel;
    ClSimState                * pclSimState;

    // Methods
public:
    bool Open(std::string sFilename);
    void Close();
    bool ReadNextLine();

    };

