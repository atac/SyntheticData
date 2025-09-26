#pragma once

#include <string>
#include <exception>

#include "GenerationControllerTypes.h"

#include "i106_stdint.h"
#include "irig106ch10.h"

#include "Config.h"
#include "SimTimer.h"
#include "TmatsFormatter.h"

#include "Source_CsvTxt.h"
#include "Source_TsvTxt.h"
#include "Source_SQLiteDB.h"

#include "Ch10Format_PCM_CSV.h"
#include "Ch10Format_PCM_SynthFmt1.h"
#include "Ch10Format_1553_Nav.h"
#include "Ch10Format_A429_AR100.h"
#include "Ch10Format_Video.h"

#include "Ch10Writer_PCM.h"
#include "Ch10Writer_1553.h"
#include "Ch10Writer_A429.h"
#include "Ch10Writer_Video.h"
#include "Ch10Writer_Index.h"
#include "Ch10Writer_Time.h"


class ControllerState
{
public:
  string programName;

  ClSimState* simState;
  ControllerTime time;

  vector<ClSource_Nav*>* sources;
  vector<Ch10Channel*>* channels;
  vector<ClSimTimer*>* timers;

  int i106OutFileHandle;

  Ch10Channel* timeChannel;

public:
  ControllerState();
  ~ControllerState();

  ControllerStatus Configure(string configFilepath);

private:

  // Initialization functions
  // ========================
  void InitControllerObjects();
  int  InitOutputFile(string directory, string filename);
  void InitTimers();
  // ========================


  // Configuration functions
  // =======================

  void                AddIndexChannel(ClCh10Writer_Time* timeWriter);
  ClCh10Writer_Time*  AddTimeChannel();
  ControllerStatus    AddDataChannel(ConfigChannel channel);

  void AddTimedChannelAction(Ch10Channel* channel, Rate rate, ChannelActionType action, bool fireImmediately = false);
  ClSimTimer* GetTimer(Rate rate, bool startExpired);
  ClSimTimer* GetExistingTimer(int64_t msTimeout, bool startExpired);
  ClSimTimer* CreateTimer(int64_t timeout, bool startExpired);
  void InsertTimer(ClSimTimer* timer);
  //Source_Nav AddSource(std::string sourcePathname);

  std::string GenerateChannelName(Ch10Channel::ChannelType type);
  Ch10Channel* CreateChannel(Ch10Writer* writer, Ch10Formatter* formatter, Ch10Channel::ChannelType type, std::string sourcePrefix, std::string name = "");
  ClSource_Nav* CreateSource(ConfigDataSource config, int channelID);

  // TODO: this should eventually take (or use) a structure containing source setup data rather than a csv source
  Ch10Formatter_PCM* CreatePcmFormatter(Ch10Channel::ChannelDataFormat format, Rate framerate, ClSource_Nav* src);
  // TODO: for general 1553 formatters, up to 6 parameters needed to define messages. None needed for hardcoded synthfmt1
  Ch10Formatter_1553* Create1553Formatter(Ch10Channel::ChannelDataFormat format, ClSource_Nav* src);
  // TODO: this should eventually take (or use) a structure containing source setup data rather than a csv source
  Ch10Formatter_ARINC429* CreateA429Formatter(Ch10Channel::ChannelDataFormat format, ClSource_Nav* src, int busSpeed, int engineNumber);
  // TODO: this should eventually take (or use) a structure containing source setup data rather than a csv source
  Ch10Formatter_Video* CreateVideoFormatter(Ch10Channel::ChannelDataFormat format, ClSource_Nav* src);

  // =======================

};

