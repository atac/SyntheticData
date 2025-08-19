#include "Ch10Channel.h"

Ch10Channel::Ch10Channel() {
  this->writer = nullptr;
  this->formatter = nullptr;
}

Ch10Channel::~Ch10Channel() {
  delete writer;
  delete formatter;
}

void Ch10Channel::Init(Ch10Writer* writer, Ch10Formatter* formatter, ChannelType type, std::string name) {
  this->writer = writer;
  this->formatter = formatter;

  this->type = type;
  this->name = name;
}

void Ch10Channel::PushData(ClSimState* simState, int64_t* clockTicks) {
  this->formatter->FormatMsg(simState);
  this->formatter->SetRTC(clockTicks);
  this->writer->AppendMsg();

void Ch10Channel::CommitPacket() {
  if (this->writer->HasMsgToWrite())
    this->writer->Commit();
}

std::string Ch10Channel::GetTMATS(ClTmatsIndexes index) {
  return this->writer->TMATS(index, this->name);
}

Ch10Channel::ChannelType Ch10Channel::Type() {
  return type;
}