#pragma once

#include "ControllerState.h"
#include "ConsoleProgressBar.h"

using namespace std;  

class GenerationController
{

  ControllerState* state;
  ConsoleProgressBar* progressBar;

public:

  GenerationController();
  ~GenerationController();

  ControllerStatus Init(string configPathname);
  ControllerStatus Fire(); // run an iteration

private:

  // Initialization functions
  // ========================
  void InitProgressBar();
  // ========================


  // Execution Functions
  // ===================
  bool UpdateSources();
  void PollTimers(); // Check all timers for expiration, and perform associated channel actions
  void DoChannelActions(vector<ChannelAction>* chanActions);
  void DoAction(ChannelAction);
  void Tick(); // update the clocks
  void UpdateProgressBar();
  // ===================
};

