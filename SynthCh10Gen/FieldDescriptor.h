#pragma once
#include <string>

using namespace std;

enum class FieldType {
  FLOAT_FIELD = 0,
  INTEGER_FIELD,
  BLOB_FIELD
};

class FieldDescriptor
{
private:
  string name;
  string id;
  FieldType type;
  string prefix;

public:
  FieldDescriptor();
  FieldDescriptor(string name);
  FieldDescriptor(string name, FieldType type);
  
  void setName(string name);
  void setIdPrefix(string prefix);
  void setType(FieldType type);

  string getName() const;
  string getID() const;
  FieldType getType() const;
};

