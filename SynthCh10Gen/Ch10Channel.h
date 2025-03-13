#pragma once

#include "Ch10Writer.h"
#include "Ch10Formatter.h"

#include "SimState.h"

class Ch10Channel
{
public:
  enum class ChannelType {
    INVALID = 0,
    A429,
    PCM,
    MS1553,
    VIDEO,
    TIME,
    INDEX
  };

  enum class ChannelDataFormat {
    INVALID = 0,
    UNFORMATTED,
    CUSTOM,
    SYNTHFORMAT1
  };

private:
  Ch10Writer* writer;
  Ch10Formatter* formatter;
  ChannelType type;
  std::string name;

public:
  Ch10Channel();
  ~Ch10Channel();

  void Init(Ch10Writer* writer, Ch10Formatter* formatter, ChannelType type, std::string name);
  void PushData(ClSimState* simState, int64_t* clockTicks); // push data from sim into packet
  void CommitPacket(); // commit full packet to file stream

  std::string GetTMATS(ClTmatsIndexes index);

  ChannelType Type();
};

