#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "sqlite3.h"

#include "SimState.h"

class ClSource_VideoDB
    {
public:
    ClSource_VideoDB();
    ~ClSource_VideoDB();

public:
    // Class variables
    int                         iStatus;
    sqlite3                   * pDB;
    std::string                 sSQL;
    sqlite3_stmt              * pSqlStmt;
    std::string                 sVideoTableName;
    int                         iCurrRowNum;
    bool                        bVideoDataValid;
    uint8_t                   * pachTSData;
    int                         iTSDataLength;

//    ClSimState                * pclSimState;

    // Methods
public:
    bool Open(std::string sFilename, std::string sVideoTableName);
    void Close();
    bool Read(int iRowNum);

    };

