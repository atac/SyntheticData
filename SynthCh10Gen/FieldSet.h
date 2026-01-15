#pragma once

#include <vector>
#include <stdexcept>
#include "FieldDescriptor.h"
#include "ConfigLib.h"

typedef vector<string> StringList;
typedef vector<FieldType> FieldTypeList;
typedef vector<FieldDescriptor> FieldDescriptorList;

class FieldSet
{
private:
  FieldDescriptorList fields;

public:
  FieldSet();
  FieldSet(StringList names);
  FieldSet(StringList names, FieldTypeList types);

  void addField(FieldDescriptor field);
  void applyMapping(GenerationConfig::ConfigMapping mapping);
  void applyPrefix(string prefix);

  StringList getFieldNames() const;
  StringList getFieldIDs() const;
  FieldTypeList getFieldTypes() const;
  FieldDescriptorList getFields() const;
  size_t size() const;

  FieldDescriptorList::const_iterator begin() const;
  FieldDescriptorList::const_iterator end() const;

  const FieldDescriptor& operator[](int index) const;
};

