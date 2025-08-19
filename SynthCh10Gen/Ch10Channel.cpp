#include "Ch10Channel.h"

Ch10Channel::Ch10Channel() {
  this->writer = nullptr;
  this->formatter = nullptr;
}

Ch10Channel::~Ch10Channel() {
  delete writer;
  delete formatter;
}

void Ch10Channel::Init(
    Ch10Writer* writer, 
    Ch10Formatter* formatter, 
    ChannelType type, 
    std::string name, 
    std::string sourcePrefix
  ) 
{
  this->writer = writer;
  this->formatter = formatter;

  this->type = type;
  this->name = name;
  this->srcPrefix = sourcePrefix;
}

void Ch10Channel::PushData(ClSimState* simState, int64_t clockTicks) {
  if (DataAvailable(simState))
  {
    this->formatter->FormatMsg(simState);
    this->formatter->SetRTC(clockTicks);
    this->writer->AppendMsg();
    UpdateAvailability(simState);
  }
}

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

bool Ch10Channel::DataAvailable(ClSimState* simState)
{
  if (this->srcPrefix.empty())
    return true;

  return simState->readyState[this->srcPrefix];
}

void Ch10Channel::UpdateAvailability(ClSimState* simState)
{
  if (!this->srcPrefix.empty())
    simState->updateReady(this->srcPrefix, false);
}