#pragma once
#include "Ch10Formatter_Video.h"

#include "csv_parser.hpp"

class Ch10Format_Video :
    public Ch10Formatter_Video
{
public:
  Ch10Format_Video(CSV_FIELDS dataLabels, CSV_FIELDS dataTypes);
  ~Ch10Format_Video() {};

  void SetRTC(int64_t* pllRelTime);
  void FormatMsg(ClSimState* simState);
  std::string TMATS(ClTmatsIndexes& tmatsIndex, std::string sCDLN);

private:
  std::string fieldName;
  void FilterFields(CSV_FIELDS labels, CSV_FIELDS types);
};

