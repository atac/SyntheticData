
#include <cassert>
#include <cstdio>
#include <string>

#include "sqlite3.h"

#include "Source_SQLiteDB.h"


// ----------------------------------------------------------------------------
// Constructor / Destructor
// ----------------------------------------------------------------------------

ClSource_SQLiteDB::ClSource_SQLiteDB(ClSimState* pclSimState, std::string sPrefix) :
  ClSource_Nav()
{
  this->pDB = NULL;
  this->pclSimState = pclSimState;
  this->sPrefix = sPrefix;
  this->enInputType = this->InputBMSqlite;
}


// ----------------------------------------------------------------------------

ClSource_SQLiteDB::~ClSource_SQLiteDB()
{
  Close();
}


// ----------------------------------------------------------------------------
// Methods
// ----------------------------------------------------------------------------

void ClSource_SQLiteDB::Config(string tableName)
{
  this->tableName = tableName;
}


/// Open the BlueMax data file

bool ClSource_SQLiteDB::Open(std::string sFilename)
    {
    int                 iStatus;

    assert(!tableName.empty());

    iStatus = sqlite3_open_v2(sFilename.c_str(), &pDB, SQLITE_OPEN_READONLY, NULL);
    if (iStatus != SQLITE_OK)
        return false;

    Init();

    return true;

    } // end Open()


// ----------------------------------------------------------------------------

/// Close the BlueMax data file

void ClSource_SQLiteDB::Close()
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

void ClSource_SQLiteDB::Init() 
{
  std::string sSQLCols = "";

  // Get the list of columns
  sSQL = "pragma table_info('" + this->tableName + "')";
  iStatus = sqlite3_prepare_v2(pDB, sSQL.c_str(), -1, &pSqlStmt, NULL);
  if (iStatus == SQLITE_OK)
  {
    // Read column labels and types from DB
    while (sqlite3_step(pSqlStmt) == SQLITE_ROW) // last iteration in lieu of calling ReadNextLine() like other sources
    {
      this->DataLabels.push_back((char*)sqlite3_column_text(pSqlStmt, 1));
      this->DataTypes.push_back((char*)sqlite3_column_text(pSqlStmt, 2));
    }

    // Make column list for SELECT statement
    for (auto i = DataLabels.begin(); i != DataLabels.end(); i++)
    {
      if (!sSQLCols.empty())
        sSQLCols.append(", ");
      sSQLCols.append(*i);
    }

    ApplyMapping();
    ApplyPrefix();
    InitSimStateFields();
  }
  sqlite3_finalize(pSqlStmt);


  // Select all the data from the BlueMax table and get ready to iterate through it.
  sSQL = "SELECT ";
  sSQL += sSQLCols;
  sSQL += " from " + this->tableName + ";";
  iStatus = sqlite3_prepare_v2(pDB, sSQL.c_str(), -1, &pSqlStmt, NULL);
  if (iStatus != SQLITE_OK)
  {
    printf("SQLite SELECT error - %s\n", sqlite3_errmsg(pDB));
    pSqlStmt = NULL;
  }
}

void ClSource_SQLiteDB::InitSimStateFields() 
{
  assert(DataLabels.size() == DataTypes.size());

  for (int i = 0; i < DataLabels.size(); i++)
  {
    string type = DataTypes[i];
    
    if (type == "INT")
      pclSimState->insert(DataLabels[i], (long)0);
    else if (type == "REAL")
      pclSimState->insert(DataLabels[i], 0.0);
    else if (type == "BLOB")
      pclSimState->insert(DataLabels[i], nullptr);
  }
}

// ----------------------------------------------------------------------------

/// Read the next line of BlueMax data

bool ClSource_SQLiteDB::ReadNextLine()
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

bool ClSource_SQLiteDB::UpdateSimState(double fSimElapsedTime)
{
  unsigned    uColIdx;
  bool        bStatus;

  // Return if simulation time is less than current data time from this source
  if (fSimElapsedTime < fRelTime)
    return true;

  // Get the individual column values
  for (uColIdx = 0; uColIdx < DataLabels.size(); uColIdx++)
  {
    // Read column based on column type
    switch (sqlite3_column_type(pSqlStmt, uColIdx))
    {
    case SQLITE_INTEGER:
      pclSimState->update(DataLabels[uColIdx], (long)sqlite3_column_int64(pSqlStmt, uColIdx));
      break;
    case SQLITE_FLOAT:
      pclSimState->update(DataLabels[uColIdx], sqlite3_column_double(pSqlStmt, uColIdx));
      break;
    case SQLITE_BLOB:
    {
      const uint8_t* blob = (uint8_t*)sqlite3_column_blob(pSqlStmt, uColIdx);
      uint32_t size = sqlite3_column_bytes(pSqlStmt, uColIdx);

      vector<uint8_t>* data;

      if (size == 0)
        data = new vector<uint8_t>();
      else
        data = new vector<uint8_t>(blob, blob + size);

      pclSimState->update(DataLabels[uColIdx], data);
      break;
    }
    default:
      break;
    } // end switch on column type
  } // end for all columns

  // Get the next line of data
  bStatus = ReadNextLine();

  return bStatus;
} // end UpdateSimState()


void ClSource_SQLiteDB::SetMapping(ConfigMapping map) {
  this->mapping = map;
}

void ClSource_SQLiteDB::ApplyMapping() {
  for (auto& [from, to] : this->mapping) {
    auto i = find(DataLabels.begin(), DataLabels.end(), from);
    if (i != DataLabels.end())
      (*i) = to;
  }
}

void ClSource_SQLiteDB::ApplyPrefix() {
  for (auto i = DataLabels.begin(); i != DataLabels.end(); i++)
    (*i) = sPrefix + (*i);
}