
#include <cassert>
#include <cstdio>
#include <string>

#include "sqlite3.h"

#include "Source_VideoDB.h"


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClSource_VideoDB::ClSource_VideoDB()
    {
    this->pDB             = NULL;
    this->pachTSData      = NULL;
    this->bVideoDataValid = false;

//    this->pclSimState = pclSimState;
    }


// ----------------------------------------------------------------------------

ClSource_VideoDB::~ClSource_VideoDB()
    {
    Close();
    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

/// Open the video database file

bool ClSource_VideoDB::Open(std::string sFilename, std::string sVideoTableName)
    {
    std::string         sSQLCols;
    int                 iStatus;

    iStatus = sqlite3_open_v2(sFilename.c_str(), &pDB, SQLITE_OPEN_READONLY, NULL);
    if (iStatus != SQLITE_OK)
        return false;

    this->sVideoTableName = sVideoTableName;

    // Select the data from the video table.
    sSQL  = "SELECT RowNum,VideoData from ";
    sSQL += sVideoTableName;
    sSQL += ";";
    iStatus = sqlite3_prepare_v2(pDB, sSQL.c_str(), -1, &pSqlStmt, NULL);
    if (iStatus != SQLITE_OK)
        {
        printf("SQLite SELECT error - %s\n", sqlite3_errmsg(pDB));
        pSqlStmt = NULL;
        return false;
        }

    iCurrRowNum = -1;

    return true;

    } // end Open()


// ----------------------------------------------------------------------------

/// Close the video database file
void ClSource_VideoDB::Close()
    {
    if (pDB != NULL)
        {
        sqlite3_finalize(pSqlStmt);
        pSqlStmt = NULL;

        sqlite3_close(pDB);
        pDB = NULL;

        bVideoDataValid = false;
        }

    return;
    } // end Close()

// ----------------------------------------------------------------------------

/// Read the video data associated with given BlueMax data row number
bool ClSource_VideoDB::Read(int iRowNum)
    {
    int         iStatus;

    // If necessary advance to the give row number
    while (iCurrRowNum < iRowNum)
        {
        // Get the next row of data
        iStatus = sqlite3_step(pSqlStmt);
        if (iStatus != SQLITE_ROW)
            {
            bVideoDataValid = false;
            return false;
            }

        iCurrRowNum = sqlite3_column_int(pSqlStmt, 0);
        }

    // If we have a row at the given row number then get the data and make it available
    if (iCurrRowNum == iRowNum)
        {
        pachTSData    = (uint8_t *)sqlite3_column_blob(pSqlStmt, 1);
        iTSDataLength =            sqlite3_column_bytes(pSqlStmt, 1);
        bVideoDataValid = true;
        return true;
        }
    else
        {
        bVideoDataValid = false;
        return false;
        }

    } // end Read()


