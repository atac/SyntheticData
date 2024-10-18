#include "Ch10Channel.h"

Ch10Channel::Ch10Channel() {
  this->writer = nullptr;
  this->formatter = nullptr;
}

Ch10Channel::~Ch10Channel() {
  delete writer;
  delete formatter;
}

void Ch10Channel::Init(Ch10Writer* writer, Ch10Formatter* formatter) {
  this->writer = writer;
  this->formatter = formatter;
}

void Ch10Channel::PushData(ClSimState* simState, int64_t* clockTicks) {
  this->formatter->MakeMsg(simState);
  this->formatter->SetRTC(clockTicks);
  this->writer->AppendMsg(this->formatter);
}

void Ch10Channel::CommitPacket() {
  this->writer->Commit();
}
