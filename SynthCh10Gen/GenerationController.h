#pragma once

#include <string>
#include <exception>

#include "GenerationControllerTypes.h"
#include "Ch10Channel.h"

#include "Source_CsvTxt.h"
#include "Ch10Format_PCM_CSV.h"
#include "Ch10Writer_PCM.h"
#include "Ch10Writer_Index.h"
#include "Ch10Writer_Time.h"
#include "SimTimer.h"
#include "irig106ch10.h"

using namespace std;

class GenerationController
{
  const int CONTROLLER_OK = 0;

  ClSimState* simState;
  ControllerTime time;

  vector<ClSource_Nav*>* sources;
  vector<Ch10Channel*>* channels;
  map<ClSimTimer*, vector<ChannelAction>*>* timers;
  //TimersMapToChannels (specify relationship action as PUSH or COMMIT)
  int outFileHandle;

public:
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

  void AddIndexWriter();
  void AddTimeWriter();
  
  // =======================

  // Initialization functions
  // ========================
  
  void InitTimers();

  // ========================

  //void UpdateSources();
  //void PollTimers(); // Check all timers for expiration, and perform associated channel actions
  //void Tick(); // update the various timers
};

