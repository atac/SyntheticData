
#include <cassert>
#include <cstdio>
#include <string>

#include "sqlite3.h"

#include "Source_BMNavDB.h"


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClSource_BMNavDB::ClSource_BMNavDB(ClSimState * pclSimState, std::string sPrefix) :
    ClSource_Nav(pclSimState, sPrefix)
    {
    this->pDB         = NULL;
    this->pclSimState = pclSimState;
    this->sPrefix     = sPrefix;
    this->enInputType = this->InputBMSqlite;
    }


// ----------------------------------------------------------------------------

ClSource_BMNavDB::~ClSource_BMNavDB()
    {
    Close();
    }


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

/// Open the BlueMax data file

bool ClSource_BMNavDB::Open(std::string sFilename)
    {
    std::string         sSQLCols;
    int                 iStatus;

    asColLabel.clear();

    iStatus = sqlite3_open_v2(sFilename.c_str(), &pDB, SQLITE_OPEN_READONLY, NULL);
    if (iStatus != SQLITE_OK)
        return false;

    // Get the list of columns
    sSQLCols.clear();
    iStatus = sqlite3_prepare_v2(pDB, "pragma table_info('BlueMax')", -1, &pSqlStmt, NULL);
    if (iStatus == SQLITE_OK)
        {
        while(sqlite3_step(pSqlStmt) == SQLITE_ROW)
            {
            // Make sure state variables exist for each column found
            std::string sDataLabel;
            sDataLabel = sPrefix + (char *)sqlite3_column_text(pSqlStmt, 1);
            if      (strcmp((char *)sqlite3_column_text(pSqlStmt, 2), "INT") == 0)
                pclSimState->insert(sDataLabel, (long)0);
            else if (strcmp((char *)sqlite3_column_text(pSqlStmt, 2), "REAL") == 0)
                pclSimState->insert(sDataLabel, 0.0);

            // Make a list of columns for a later SELECT statement
            if (sSQLCols.length() != 0)
                sSQLCols += ", ";
            asColLabel.insert(asColLabel.end(), (char *)sqlite3_column_text(pSqlStmt, 1));
            sSQLCols += (char *)sqlite3_column_text(pSqlStmt, 1);
            }
        }
    sqlite3_finalize(pSqlStmt);

    // Select all the data from the BlueMax table and get ready to iterate through it.
    sSQL  = "SELECT ";
    sSQL += sSQLCols;
    sSQL += " from BlueMax;";
    iStatus = sqlite3_prepare_v2(pDB, sSQL.c_str(), -1, &pSqlStmt, NULL);
    if (iStatus != SQLITE_OK)
        {
        printf("SQLite SELECT error - %s\n", sqlite3_errmsg(pDB));
        pSqlStmt = NULL;
        }

    return true;

    } // end Open()


// ----------------------------------------------------------------------------

/// Close the BlueMax data file

void ClSource_BMNavDB::Close()
    {
    if (pDB != NULL)
        {
        sqlite3_finalize(pSqlStmt);
        pSqlStmt = NULL;
        sqlite3_close(pDB);
        pDB = NULL;
        }

    return;
    } // end Close()

// ----------------------------------------------------------------------------

/// Read the next line of BlueMax data

bool ClSource_BMNavDB::ReadNextLine()
{
  int         iStatus;

  // Get the next row of data
  iStatus = sqlite3_step(pSqlStmt);
  if (iStatus != SQLITE_ROW)
    return false;

  // Assume the BMdb actime column is index one and represents seconds since 0.0
  fRelTime = sqlite3_column_double(pSqlStmt, 1);

  return true;
}


// ----------------------------------------------------------------------------

bool ClSource_BMNavDB::UpdateSimState(double fSimElapsedTime)
{
  unsigned    uColIdx;
  bool        bStatus;

  // Return if simulation time is less than current data time from this source
  if (fSimElapsedTime < fRelTime)
    return true;

  // Get the individual column values
  for (uColIdx = 0; uColIdx < asColLabel.size(); uColIdx++)
  {
    // Read column based on column type
    switch (sqlite3_column_type(pSqlStmt, uColIdx))
    {
    case SQLITE_INTEGER:
      pclSimState->update(sPrefix + asColLabel[uColIdx], (long)sqlite3_column_int64(pSqlStmt, uColIdx));
      break;
    case SQLITE_FLOAT:
      pclSimState->update(sPrefix + asColLabel[uColIdx], sqlite3_column_double(pSqlStmt, uColIdx));
      break;
    default:
      break;
    } // end switch on column type
  } // end for all columns

// Get the next line of data
  bStatus = ReadNextLine();

  return bStatus;
} // end UpdateSimState()
