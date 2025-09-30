#include "GenerationController.h"


// TODO:
// Decide if we need a case for poll and packet rate being "on demand"
// Should we find a way to combine sources for channels that use the same source?


GenerationController::GenerationController() {
  state = nullptr;
  progressBar = nullptr;
}

GenerationController::~GenerationController() {
  if (state != nullptr)
    delete state;

  if (progressBar != nullptr)
    delete progressBar;
}

ControllerStatus GenerationController::Init(string configPathname, bool validateOnly) {
  state = new ControllerState();
  ControllerStatus result = state->Configure(configPathname, validateOnly);

  if (state->configLog.size() != 0)
    cout << "Validation " << state->configLog << endl;

  if (result != ControllerStatus::OK || validateOnly)
    return result;

  try {
    InitProgressBar();
    UpdateProgressBar();
  }
  catch (exception ex) {
    printf(ex.what());
    result = ControllerStatus::INVALID_PROGRESS_PARAMETERS;
  }

  return result;
}

ControllerStatus GenerationController::Fire() {
  if (!UpdateSources())
    return ControllerStatus::SOURCES_DEPLETED;
  PollTimers();
  Tick();

  UpdateProgressBar();

  return ControllerStatus::OK;
}

bool GenerationController::UpdateSources() {
  bool sourceIsReady = false;

  for (auto s : *state->sources) {
    if (s->UpdateSimState(ClSimTimer::fSimElapsedTime))
      sourceIsReady = true;
  }

  return sourceIsReady;
}

void GenerationController::PollTimers() {
  for (auto t : *state->timers) {
    if (t->Expired()) {
      t->FromPrev();
      DoChannelActions(t->actions);
    }
  }
}

void GenerationController::DoChannelActions(vector<ChannelAction>* chanActions) {
  for (auto ca : *chanActions) {
    DoAction(ca);
  }
}

void GenerationController::DoAction(ChannelAction chanAction) {
  switch (chanAction.type) {

  case ChannelActionType::PUSH:
    chanAction.channel->PushData(state->simState, ClSimTimer::lSimClockTicks);
    break;

  case ChannelActionType::COMMIT:
    chanAction.channel->CommitPacket();
    break;

  }
}

void GenerationController::Tick() {
  ClSimTimer::Tick();
  state->time.currSimClockTime = state->time.startSimClockTime + ClSimTimer::fSimElapsedTime;
}

void GenerationController::UpdateProgressBar()
{
  progressBar->SetProgress(state->time.currSimClockTime);
  string bar = "\r" + progressBar->GetBar();
  printf(bar.data());
}

void GenerationController::InitProgressBar() {
  double startTime = state->time.startSimClockTime;
  double endTime = 0.0;

  for (auto src : (*state->sources)) {
    if (src->fEndTime > endTime)
      endTime = src->fEndTime;
  }

  progressBar = new ConsoleProgressBar(startTime, endTime);
}
