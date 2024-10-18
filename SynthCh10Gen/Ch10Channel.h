#pragma once

#include "Ch10Writer.h"
#include "Ch10Formatter.h"

#include "SimState.h"

class Ch10Channel
{
  Ch10Writer* writer;
  Ch10Formatter* formatter;

public:
  Ch10Channel();
  ~Ch10Channel();

  void Init(Ch10Writer* writer, Ch10Formatter* formatter);
  void PushData(ClSimState* simState, int64_t* clockTicks); // push data from sim into packet
  void CommitPacket(); // commit full packet to file stream
};

