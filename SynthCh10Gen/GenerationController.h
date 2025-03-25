#pragma once

#include <string>
#include <exception>

#include "GenerationControllerTypes.h"
#include "Ch10Channel.h"

#include "Source_CsvTxt.h"

#include "Ch10Format_PCM_CSV.h"
#include "Ch10Format_PCM_SynthFmt1.h"
#include "Ch10Format_1553_Nav.h"
#include "Ch10Format_A429_AR100.h"

#include "Ch10Writer_PCM.h"
#include "Ch10Writer_1553.h"
#include "Ch10Writer_A429.h"
#include "Ch10Writer_Index.h"
#include "Ch10Writer_Time.h"

#include "SimTimer.h"

#include "i106_stdint.h"
#include "irig106ch10.h"

#include "TmatsFormatter.h"
#include "Config.h"

using namespace std;  

class GenerationController
{

  string programName;

  ClSimState* simState;
  ControllerTime time;

  vector<ClSource_Nav*>* sources;
  vector<Ch10Channel*>* channels;
  vector<ClSimTimer*>* timers;

  int i106OutFileHandle;

  Ch10Channel* timeChannel;


public:

  GenerationController();
  ~GenerationController();

  ControllerStatus Init(string configPathname);
  ControllerStatus Fire(); // run an iteration

private:
  ControllerStatus ReadConfig(std::string configFilepath);


  // Configuration functions
  // =======================

  void AddTimedChannelAction(Ch10Channel* channel, Rate rate, ChannelActionType action);
  ClSimTimer* GetTimer(Rate rate);
  ClSimTimer* GetExistingTimer(int64_t msTimeout);
  ClSimTimer* CreateTimer(int64_t timeout);
  void InsertTimer(ClSimTimer* timer);
  //Source_Nav AddSource(std::string sourcePathname);

  void                AddIndexChannel(ClCh10Writer_Time* timeWriter);
  ClCh10Writer_Time*  AddTimeChannel();
  ControllerStatus    AddDataChannel(ConfigChannel channel);

  std::string GenerateChannelName(Ch10Channel::ChannelType type);
  Ch10Channel* CreateChannel(Ch10Writer* writer, Ch10Formatter* formatter, Ch10Channel::ChannelType type, std::string name = "");

  // TODO: this should eventually take (or use) a structure containing source setup data rather than a csv source
  Ch10Formatter_PCM* CreatePcmFormatter(Ch10Channel::ChannelDataFormat format, Rate framerate, ClSource_CsvTxt* src);
  // TODO: for general 1553 formatters, up to 6 parameters needed to define messages. None needed for hardcoded synthfmt1
  Ch10Formatter_1553* Create1553Formatter(Ch10Channel::ChannelDataFormat format, ClSource_CsvTxt* src);
  // TODO: this should eventually take (or use) a structure containing source setup data rather than a csv source
  Ch10Formatter_ARINC429* CreateA429Formatter(Ch10Channel::ChannelDataFormat format, ClSource_CsvTxt* src, int busSpeed, int engineNumber);
  
  // =======================


  // Initialization functions
  // ========================
  
  void InitControllerObjects();
  int  InitOutputFile(string directory, string filename);
  void InitTimers();

  // ========================


  // Execution Functions
  // ===================
  bool UpdateSources();
  void PollTimers(); // Check all timers for expiration, and perform associated channel actions
  void DoChannelActions(vector<ChannelAction>* chanActions);
  void DoAction(ChannelAction);
  void Tick(); // update the clocks
  // ===================
};

