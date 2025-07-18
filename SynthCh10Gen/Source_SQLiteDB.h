#pragma once

#include <string>
#include <vector>
#include <unordered_map>

#include "sqlite3.h"

#include "Source_Nav.h"
#include "Source_VideoDB.h"
#include "SimState.h"

class ClSource_SQLiteDB :
  public ClSource_Nav
{
public:
  ClSource_SQLiteDB(ClSimState* pclSimState, std::string sPrefix);
  ~ClSource_SQLiteDB();

public:
  // Class variables
  int             iStatus;
  sqlite3*        pDB;
  std::string     sSQL;
  sqlite3_stmt*   pSqlStmt;

  // Methods
public:
  virtual bool    Open(std::string sFilename);
  virtual void    Close();
  virtual bool    ReadNextLine();
  virtual bool    UpdateSimState(double fSimElapsedTime);
  virtual void    SetMapping(ConfigMapping map);

  void Config(string tableName);

private:
  virtual void ApplyMapping();
  virtual void ApplyPrefix();

  void InitSimStateFields();
  void Init();

  string tableName;
};
