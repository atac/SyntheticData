
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
  this->eof = false;

  this->fTimeShift = 0.0;
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
    StringList fieldNames;
    FieldTypeList fieldTypes;

    // Read column labels and types from DB
    while (sqlite3_step(pSqlStmt) == SQLITE_ROW)
    {
      fieldNames.push_back((char*)sqlite3_column_text(pSqlStmt, 1));
      fieldTypes.push_back(GetFieldType((char*)sqlite3_column_text(pSqlStmt, 2)));
    }

    // Make column list for SELECT statement
    for (auto i = fieldNames.begin(); i != fieldNames.end(); i++)
    {
      if (!sSQLCols.empty())
        sSQLCols.append(", ");
      sSQLCols.append(*i);
    }

    fields = FieldSet(fieldNames, fieldTypes);
    fields.applyMapping(this->mapping);
    fields.applyPrefix(this->sPrefix);

    InitSimStateFields();
  }
  sqlite3_finalize(pSqlStmt);

  
  GetTimes();


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

  ReadNextLine(); // prep the data for the first iteration
}

bool ClSource_SQLiteDB::GetTimes()
{
  // Find first timestamp
  string sql = "SELECT * from " + this->tableName + " ORDER BY RowNum ASC LIMIT 1;";
  int iStatus = sqlite3_prepare_v2(pDB, sql.c_str(), -1, &pSqlStmt, NULL);
  if (iStatus != SQLITE_OK)
  {
    printf("SQLite SELECT error - %s\n", sqlite3_errmsg(pDB));
    return false;
  }

  iStatus = sqlite3_step(pSqlStmt);
  if (iStatus != SQLITE_ROW)
    return false;

  // First column must be time
  fStartTime = sqlite3_column_double(pSqlStmt, 1);

  sqlite3_finalize(pSqlStmt);


  // Find last timestamp
  sql = "SELECT * from " + this->tableName + " ORDER BY RowNum DESC LIMIT 1;";
  iStatus = sqlite3_prepare_v2(pDB, sql.c_str(), -1, &pSqlStmt, NULL);
  if (iStatus != SQLITE_OK)
  {
    printf("SQLite SELECT error - %s\n", sqlite3_errmsg(pDB));
    return false;
  }

  iStatus = sqlite3_step(pSqlStmt);
  if (iStatus != SQLITE_ROW)
    return false;

  // First column must be time
  fEndTime = sqlite3_column_double(pSqlStmt, 1);

  sqlite3_finalize(pSqlStmt);

  return true;
}

void ClSource_SQLiteDB::InitSimStateFields() 
{
  for (auto i = fields.begin(); i != fields.end(); i++)
  {
    switch (i->getType()) {
    case FieldType::INTEGER_FIELD:
      pclSimState->insert(i->getID(), (long)0);
      break;
    case FieldType::BLOB_FIELD:
      pclSimState->insert(i->getID(), nullptr);
      break;
    case FieldType::FLOAT_FIELD:
    default:
      pclSimState->insert(i->getID(), 0.0);
      break;
    }
  }

  if (!sPrefix.empty())
    pclSimState->insertReady(sPrefix);
}

// ----------------------------------------------------------------------------

/// Read the next line of BlueMax data

bool ClSource_SQLiteDB::ReadNextLine()
{
  bool dataAvailable = false;

  int status = sqlite3_step(pSqlStmt);
  if (status == SQLITE_ROW)
  {
    // First column must be time
    double time = sqlite3_column_double(pSqlStmt, 1);
    fRelTime = time - fStartTime;
    dataAvailable = true;
  }
  else if (status == SQLITE_DONE)
    eof = true;

  return dataAvailable;
}


// ----------------------------------------------------------------------------

bool ClSource_SQLiteDB::UpdateSimState(double fSimElapsedTime)
{
  bool bStatus = true;

  fSimElapsedTime -= fTimeShift; // apply shift by changing the apparent elapsed time


  while (fSimElapsedTime + TIME_COMPARE_MARGIN >= fRelTime) {
    if (eof)
      return false;

    // Get the individual column values
    for (int uColIdx = 0; uColIdx < fields.size(); uColIdx++)
    {
      // Read column based on column type
      switch (sqlite3_column_type(pSqlStmt, uColIdx))
      {
      case SQLITE_INTEGER:
        pclSimState->update(fields[uColIdx].getID(), (long)sqlite3_column_int64(pSqlStmt, uColIdx));
        break;
      case SQLITE_FLOAT:
        pclSimState->update(fields[uColIdx].getID(), sqlite3_column_double(pSqlStmt, uColIdx));
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

        pclSimState->update(fields[uColIdx].getID(), data);
        break;
      }
      default:
        break;
      } // end switch on column type
    } // end for all columns

    // Get the next line of data
    bStatus = ReadNextLine();

    pclSimState->updateReady(sPrefix, bStatus);
  }


  return bStatus;
} // end UpdateSimState()


void ClSource_SQLiteDB::SetMapping(ConfigMapping map) {
  this->mapping = map;
}


FieldType ClSource_SQLiteDB::GetFieldType(string typeString) {
  if (typeString == "INT")
    return FieldType::INTEGER_FIELD;
  else if (typeString == "BLOB")
    return FieldType::BLOB_FIELD;
  else // "REAL"
    return FieldType::FLOAT_FIELD;
}