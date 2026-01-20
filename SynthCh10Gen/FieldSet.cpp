#include "FieldSet.h"

FieldSet::FieldSet() {
}

FieldSet::FieldSet(StringList names) {
  for (string n : names)
    addField(FieldDescriptor(n));
}

FieldSet::FieldSet(StringList names, FieldTypeList types) {
  if (names.size() != types.size())
    throw invalid_argument("Argument lists must be the same size for FieldSet(2) constructor");

  for (int i = 0; i < names.size(); i++)
    addField(FieldDescriptor(names[i], types[i]));
}

void FieldSet::addField(FieldDescriptor field) {
  fields.push_back(field);
}

void FieldSet::applyMapping(GenerationConfig::ConfigMapping mapping)
{
  for (auto& [from, to] : mapping) {
    for (int i = 0; i < fields.size(); i++) {
      if (fields[i].getName() == from) {
        fields[i].setName(to);
      }
    }
  }
}

void FieldSet::applyPrefix(string prefix)
{
  for (auto& f : fields) {
    f.setIdPrefix(prefix);
  }
}

StringList FieldSet::getFieldNames() const
{
  StringList names;

  for (auto i = fields.begin(); i != fields.end(); i++)
    names.push_back(i->getName());

  return names;
}

StringList FieldSet::getFieldIDs() const
{
  StringList ids;

  for (auto i = fields.begin(); i != fields.end(); i++)
    ids.push_back(i->getID());

  return ids;
}

std::vector<FieldType> FieldSet::getFieldTypes() const
{
  std::vector<FieldType> types;

  for (auto i = fields.begin(); i != fields.end(); i++)
    types.push_back(i->getType());

  return types;
}

std::vector<FieldDescriptor> FieldSet::getFields() const {
  return fields;
}

size_t FieldSet::size() const
{
    return fields.size();
}

FieldDescriptorList::const_iterator FieldSet::begin() const {
  return fields.cbegin(); 
}

FieldDescriptorList::const_iterator FieldSet::end() const {
  return fields.cend(); 
}

const FieldDescriptor& FieldSet::operator[](int index) const {
  if (index < 0 || index >= fields.size())
    throw std::out_of_range("FieldSet access out_of_range");

  return fields[index];
}