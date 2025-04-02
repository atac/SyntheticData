#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "sqlite3.h"

#include "Source_Nav.h"
#include "SimState.h"

class ClSource_SQLiteDB :
  public ClSource_Nav
{
public:
  ClSource_SQLiteDB(ClSimState* pclSimState, std::string sPrefix);
  ~ClSource_SQLiteDB();

public:
  // Class variables
  int                         iStatus;
  sqlite3* pDB;
  std::string                 sSQL;
  sqlite3_stmt* pSqlStmt;

  std::vector<std::string>    asColLabel;

  // Methods
public:
  bool    Open(std::string sFilename);
  void    Close();
  bool    ReadNextLine();
  bool    UpdateSimState(double fSimElapsedTime);
  void    SetMapping(ConfigMapping map);

  void Config(string tableName);

private:
  string tableName;
};

