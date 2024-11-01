#include "GenerationController.h"


// TODO:
// Need more time conversions

GenerationController::GenerationController() {
  simState = nullptr;
  sources = nullptr;
  channels = nullptr;
  timers = nullptr;

  outFileHandle = 0;

  time.srcTime = 0.0;
  time.startSimClockTime = -1.0;

  programName = "Synthetic Chapter 10";
}

GenerationController::~GenerationController() {
  if (simState != nullptr)
    delete simState;

  if (sources != nullptr)
    delete sources;

  if (channels != nullptr)
    delete channels;

  if (timers != nullptr)
    delete timers;
}

int GenerationController::Init(string configPathname) {
  // TODO: open config file
  int result = TmpCreateConfig();
  if (result != CONTROLLER_OK)
    return result;

  InitTimers();

  this->time.currSimClockTime = this->time.startSimClockTime;
  TmatsFormatter::WriteTMATS(outFileHandle, programName, this->time.currSimClockTime, this->channels);
  timeChannel->PushData(simState, &ClSimTimer::lSimClockTicks);
  timeChannel->CommitPacket();

  return result;
}

bool GenerationController::Fire() {
  UpdateSources();
  PollTimers();
  Tick();
}

bool GenerationController::UpdateSources() {
  for (auto s : *sources) {
    if (!s->UpdateSimState(time.srcTime))
      return false;
  }

  return true;
}

void GenerationController::PollTimers() {
  for (auto t : *timers) {
    if (t.first->Expired()) {
      t.first->FromPrev();
      DoChannelActions(t.second);
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
    chanAction.channel->PushData(simState, &ClSimTimer::lSimClockTicks);
    break;

  case ChannelActionType::COMMIT:
    chanAction.channel->CommitPacket();
    break;

  }
}

void GenerationController::Tick() {
  ClSimTimer::Tick();
  time.srcTime = ClSimTimer::fSimElapsedTime;
  time.currSimClockTime = time.startSimClockTime + ClSimTimer::fSimElapsedTime;
}

void GenerationController::InitTimers() {
  for (auto timerAction : *timers) {
    timerAction.first->FromNow();
  }
}

int GenerationController::TmpCreateConfig() {
  // from each config item
  string configProgramName = "Fully Generic Data"; 
  string configFileSource = "pathtofile";
  Rate configDataRate(100, RateType::FREQUENCY); // frequency of frames in a packet
  Rate configPacketRate(10, RateType::FREQUENCY); // frequency of packets in a file
  Rate configIndexAppendRate(1000, RateType::TIME);
  Rate configIndexCommitRate(6000, RateType::TIME);
  int configIndexNodesPerRoot = 10;
  string configOutfile = "pathtofile";
  unsigned int configChanID = 20;
  string configChanName = "mychannel";
  Ch10Channel::ChannelType configChanType = Ch10Channel::ChannelType::PCM;

  // init structures
  sources = new std::vector<ClSource_Nav*>();
  channels = new vector<Ch10Channel*>();
  timers = new map<ClSimTimer*, vector<ChannelAction>*>();

  // init simstate object
  simState = new ClSimState();
  simState->clear();
  simState->SetSimClockTime(&(this->time.currSimClockTime));


  // create output file
  EnI106Status enStatus = enI106Ch10Open(&outFileHandle, configOutfile.data(), I106_OVERWRITE);
  if (enStatus != I106_OK)
  {
    fprintf(stderr, "Error opening data file : Status = %d\n", enStatus);
    return 1;
  }

  // create required ch10 channel writers
  ClCh10Writer_Time* timeWriter = AddTimeWriter();
  AddIndexWriter(configIndexAppendRate, configIndexCommitRate, configIndexNodesPerRoot, timeWriter);

  // create source(s) passing simstate
  int sourceCount = 0;

  // FOREACH SOURCE
  //
  // if (configLine.type == CSV)
  string sourcePrefix = "src" + (++sourceCount);
  ClSource_CsvTxt* csvSrc = new ClSource_CsvTxt(simState, sourcePrefix);
  ClSource_Nav* navSrc = dynamic_cast<ClSource_Nav*>(csvSrc);
  if (navSrc != nullptr) {
    if (!navSrc->Open(configFileSource)) {
      fprintf(stderr, "Failed to open source");
      return 1;
    }
    sources->push_back(navSrc);
  }

  // get start time from source
  time.startSimClockTime = csvSrc->fStartTime;

  // create formatter
  ClCh10Format_PCM_SynthFmtCsv* formatCsv =
    new ClCh10Format_PCM_SynthFmtCsv(
      configDataRate.value,
      csvSrc->GetCsvFields(),
      csvSrc->GetCsvFieldTypes()
    );
  Ch10Formatter* formatter = dynamic_cast<Ch10Formatter*>(formatCsv);

  // create writer
  ClCh10Writer_PCM* writerPcm = new ClCh10Writer_PCM();
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(writerPcm);
  writer->Init(outFileHandle, configChanID);

  // create channel passing formatter/writer
  Ch10Channel* channel = CreateChannel(writer, formatter, configChanType, configChanName);

  // create timers with maps to channel/action
  AddTimedChannelAction(channel, configDataRate, ChannelActionType::PUSH);
  AddTimedChannelAction(channel, configPacketRate, ChannelActionType::COMMIT);
  //
  // END FOREACH SOURCE

  timeWriter->SetRelTime(ClSimTimer::lSimClockTicks, this->time.startSimClockTime);

  return 0;
}

void GenerationController::AddIndexWriter(Rate indexRate, Rate nodeRate, uint8_t nodesPerRoot, ClCh10Writer_Time* timeWriter) {
  Ch10Format_Index* formatter = new Ch10Format_Index();
  formatter->Init(&(timeWriter->suWritePktTimeF1.suCh10Header));

  ClCh10Writer_Index* indexWriter = new ClCh10Writer_Index();
  indexWriter->Init(outFileHandle, 0, 10, formatter);
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(indexWriter);

  Ch10Channel::ChannelType type = Ch10Channel::ChannelType::Index;

  Ch10Channel* channel = CreateChannel(writer, formatter, type);

  AddTimedChannelAction(channel, indexRate, ChannelActionType::PUSH);
  AddTimedChannelAction(channel, nodeRate, ChannelActionType::COMMIT);
}

ClCh10Writer_Time* GenerationController::AddTimeWriter() {
  Ch10Format_Time* formatter = new Ch10Format_Time();

  ClCh10Writer_Time* timeWriter = new ClCh10Writer_Time();
  timeWriter->Init(outFileHandle, 1, formatter);
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(timeWriter);

  Ch10Channel::ChannelType type = Ch10Channel::ChannelType::Time;

  this->timeChannel = CreateChannel(writer, formatter, type);
  channels->push_back(this->timeChannel);

  Rate timeRate = Rate(1000, RateType::TIME);

  AddTimedChannelAction(this->timeChannel, timeRate, ChannelActionType::COMMIT);
}

void GenerationController::AddTimedChannelAction(Ch10Channel* channel, Rate rate, ChannelActionType actionType) {
  ClSimTimer* timer = GetTimer(rate);
  ChannelAction action(channel, actionType);
  timers->find(timer)->second->push_back(action);
}

// Get or create timer based on the specified rate
ClSimTimer* GenerationController::GetTimer(Rate rate) {
  if (rate.type == RateType::FREQUENCY)
    ConvertRateUnits(rate);

  ClSimTimer* timer = GetExistingTimer(rate.value);

  if (timer == nullptr)
    timer = CreateTimer(rate.value);

  return timer;
}

// Check if a timer with the same timeout already exists
ClSimTimer* GenerationController::GetExistingTimer(int64_t timeout) {
  for (auto timerPair : *timers) {
    if (timerPair.first->GetTimeoutValue() == timeout) {
      return timerPair.first;
    }
  }
  return nullptr;
}

// Create timer and add entry in map
ClSimTimer* GenerationController::CreateTimer(int64_t timeout) {
  ClSimTimer* timer = new ClSimTimer(timeout);
  vector<ChannelAction>* relationships = new vector<ChannelAction>();
  pair<ClSimTimer*, vector<ChannelAction>*> timerPair(timer, relationships);
  timers->insert(timerPair);
}

std::string GenerationController::GenerateChannelName(Ch10Channel::ChannelType type) {
  static std::map<Ch10Channel::ChannelType, int> typeCounts;

  std::string prefix = "";

  int count = 1;

  auto c = typeCounts.find(type);
  if (c != typeCounts.end()) {
    c->second++;
    count = c->second;
  }
  else
    typeCounts.insert(std::pair<Ch10Channel::ChannelType, int>(type, 1));

  switch (type) {
  case Ch10Channel::ChannelType::A429:
    prefix = "A429";
    break;
  case Ch10Channel::ChannelType::PCM:
    prefix = "PCM";
    break;
  case Ch10Channel::ChannelType::MS1553:
    prefix = "1553";
    break;
  case Ch10Channel::ChannelType::Video:
    prefix = "Video";
    break;
  case Ch10Channel::ChannelType::Time:
    prefix = "Time";
    break;
  case Ch10Channel::ChannelType::Index:
    prefix = "Index";
    break;
  }

  return prefix + "InChan" + to_string(count);
}

// Create a new, initialized channel and add it to the channels list
Ch10Channel* GenerationController::CreateChannel(
  Ch10Writer* writer, 
  Ch10Formatter* formatter,
  Ch10Channel::ChannelType type,
  std::string name) 
{
  if (name == "")
    name = GenerateChannelName(type);

  Ch10Channel* channel = new Ch10Channel();
  channels->push_back(channel);
  channel->Init(writer, formatter, type, name);
  return channel;
}