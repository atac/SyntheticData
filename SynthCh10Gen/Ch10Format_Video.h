#pragma once
#include "Ch10Formatter_Video.h"

#include "csv_parser.hpp"

class Ch10Format_Video :
    public Ch10Formatter_Video
{
public:
  Ch10Format_Video(FieldSet fields);
  ~Ch10Format_Video() {};

  void SetRTC(int64_t relTime);
  void FormatMsg(ClSimState* simState);
  std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN);

private:
  FieldDescriptor videoField;
  void FilterFields(FieldSet fields);
};

