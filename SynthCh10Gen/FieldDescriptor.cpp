#include "FieldDescriptor.h"

FieldDescriptor::FieldDescriptor() {
}

FieldDescriptor::FieldDescriptor(string name) {
  this->name = name;
  this->id = name;
  this->type = FieldType::FLOAT_FIELD;
}

FieldDescriptor::FieldDescriptor(string name, FieldType type) {
  this->name = name;
  this->id = name;
  this->type = type;
}

void FieldDescriptor::setName(string name) {
  this->name = name;
  setIdPrefix(this->prefix);
}
void FieldDescriptor::setIdPrefix(string prefix) {
  this->prefix = prefix;
  id = prefix + name;
}

void FieldDescriptor::setType(FieldType type) {
  this->type = type;
}

string FieldDescriptor::getName() const {
  return name;
}

string FieldDescriptor::getID() const {
  return id;
}

FieldType FieldDescriptor::getType() const {
  return this->type;
}
