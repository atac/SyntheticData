#pragma once

#include <string>
#include <exception>

#include "GenerationControllerTypes.h"
#include "Ch10Channel.h"

#include "Source_CsvTxt.h"
#include "Ch10Writer.h"
#include "Ch10Format_PCM_CSV.h"
#include "Ch10Writer_PCM.h"
#include "Ch10Writer_Index.h"
#include "Ch10Writer_Time.h"
#include "SimTimer.h"

#include "i106_stdint.h"
#include "irig106ch10.h"

#include "TmatsFormatter.h"

using namespace std;

class GenerationController
{

  string programName;

  ClSimState* simState;
  ControllerTime time;

  vector<ClSource_Nav*>* sources;
  vector<Ch10Channel*>* channels;
  map<ClSimTimer*, vector<ChannelAction>*>* timers;
  int outFileHandle;

  Ch10Channel* timeChannel;


public:
  static const int CONTROLLER_OK = 0;

  GenerationController();
  ~GenerationController();

  int Init(string configPathname);
  bool Fire(); // run an iteration

private:
  //void ReadConfig(std::string configPathname);
  int TmpCreateConfig(); // temporary function to demonstrate a single configuration process


  // Configuration functions
  // =======================

  void AddTimedChannelAction(Ch10Channel* channel, Rate rate, ChannelActionType action);
  ClSimTimer* GetTimer(Rate rate);
  ClSimTimer* GetExistingTimer(int64_t timeout);
  ClSimTimer* CreateTimer(int64_t timeout);
  //Source_Nav AddSource(std::string sourcePathname);
  //Ch10Channel AddChannel(void channelConfig); 

  void                AddIndexWriter(Rate indexRate, Rate nodeRate, uint8_t nodesPerRoot, ClCh10Writer_Time* timeWriter);
  ClCh10Writer_Time*  AddTimeWriter();

  std::string GenerateChannelName(Ch10Channel::ChannelType type);
  Ch10Channel* CreateChannel(Ch10Writer* writer, Ch10Formatter* formatter, Ch10Channel::ChannelType type, std::string name = "");
  
  // =======================


  // Initialization functions
  // ========================
  
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

