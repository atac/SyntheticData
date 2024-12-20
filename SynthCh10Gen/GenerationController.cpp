#include "GenerationController.h"


// TODO:
// Need more time conversions

GenerationController::GenerationController() {
  simState = nullptr;
  sources = nullptr;
  channels = nullptr;
  timers = nullptr;

  i106OutFileHandle = -1;

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

ControllerStatus GenerationController::Init(string configPathname) {
  // TODO: open config file
  ControllerStatus result = ReadConfig(configPathname);
  if (result != ControllerStatus::OK)
    return result;

  this->time.currSimClockTime = this->time.startSimClockTime;
  InitTimers();

  TmatsFormatter::WriteTMATS(i106OutFileHandle, programName, this->time.currSimClockTime, this->channels);
  timeChannel->PushData(simState, &ClSimTimer::lSimClockTicks);
  timeChannel->CommitPacket();

  return result;
}

ControllerStatus GenerationController::Fire() {
  if (!UpdateSources())
    return ControllerStatus::SOURCES_DEPLETED;
  PollTimers();
  Tick();

  return ControllerStatus::OK;
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
  for (auto t : *timers) {
    t->FromNow();
  }
}

ControllerStatus GenerationController::ReadConfig(string configFilepath) {
  Config config = Config(configFilepath);

  if (!config.Valid())
    return ControllerStatus::INVALID_CONFIG;
  
    InitControllerObjects();


  return ControllerStatus::OK;
  // TODO
}

void GenerationController::InitControllerObjects() {
  // init structures
  sources = new std::vector<ClSource_Nav*>();
  channels = new vector<Ch10Channel*>();
  timers = new vector<ClSimTimer*>();

  // init simstate object
  simState = new ClSimState();
  simState->clear();
  simState->SetSimClockTime(&(this->time.currSimClockTime));
}

int GenerationController::TmpCreateConfig() {
  // from each config item
  string configProgramName = "Fully Generic Data"; 
  string configFileSource = "C:\\atac\\vsprojects\\SyntheticData\\Debug\\30931-small.csv";
  Rate configDataRate(100, RateUnit::FREQUENCY); // frequency of frames in a packet
  Rate configPacketRate(20, RateUnit::FREQUENCY); // frequency of packets in a file
  Rate configIndexAppendRate(1000, RateUnit::TIME_MS);
  Rate configIndexCommitRate(6000, RateUnit::TIME_MS);
  int configIndexNodesPerRoot = 10;
  string configOutfile = "C:\\atac\\vsprojects\\SyntheticData\\Debug\\test_generic_pcm.ch10";
  unsigned int configChanID = 20;
  string configChanName = "genericPcmChan";
  Ch10Channel::ChannelType configChanType = Ch10Channel::ChannelType::PCM;

  // init structures
  sources = new std::vector<ClSource_Nav*>();
  channels = new vector<Ch10Channel*>();
  timers = new vector<ClSimTimer*>();

  // init simstate object
  simState = new ClSimState();
  simState->clear();
  simState->SetSimClockTime(&(this->time.currSimClockTime));


  // create output file
  EnI106Status enStatus = enI106Ch10Open(&i106OutFileHandle, configOutfile.data(), I106_OVERWRITE);
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

  // open source file
  string sourcePrefix = "src" + to_string(++sourceCount);
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
  writerPcm->Init(i106OutFileHandle, configChanID, formatCsv);
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(writerPcm);

  // create channel passing formatter/writer
  Ch10Channel* channel = CreateChannel(writer, formatter, configChanType, configChanName);

  // create timers for channel actions
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
  indexWriter->Init(i106OutFileHandle, 0, 10, formatter);
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(indexWriter);

  Ch10Channel::ChannelType type = Ch10Channel::ChannelType::Index;

  Ch10Channel* channel = CreateChannel(writer, formatter, type);

  AddTimedChannelAction(channel, indexRate, ChannelActionType::PUSH);
  AddTimedChannelAction(channel, nodeRate, ChannelActionType::COMMIT);
}

ClCh10Writer_Time* GenerationController::AddTimeWriter() {
  Ch10Format_Time* formatter = new Ch10Format_Time();

  ClCh10Writer_Time* timeWriter = new ClCh10Writer_Time();
  timeWriter->Init(i106OutFileHandle, 1, formatter);
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(timeWriter);

  Ch10Channel::ChannelType type = Ch10Channel::ChannelType::Time;

  this->timeChannel = CreateChannel(writer, formatter, type);

  Rate timeRate = Rate(1000, RateUnit::TIME_MS);

  AddTimedChannelAction(this->timeChannel, timeRate, ChannelActionType::PUSH);
  AddTimedChannelAction(this->timeChannel, timeRate, ChannelActionType::COMMIT);

  return timeWriter;
}

void GenerationController::AddTimedChannelAction(Ch10Channel* channel, Rate rate, ChannelActionType actionType) {
  ChannelAction action(channel, actionType);
  ClSimTimer* timer = GetTimer(rate);
  timer->AddAction(action);
}

// Get or create timer based on the specified rate
ClSimTimer* GenerationController::GetTimer(Rate rate) {
  rate.ConvertUnits(RateUnit::TIME_RTC);

  ClSimTimer* timer = GetExistingTimer(rate.value);

  if (timer == nullptr)
    timer = CreateTimer(rate.value);

  return timer;
}

// Check if a timer with the same timeout already exists
ClSimTimer* GenerationController::GetExistingTimer(int64_t rtcTimeout) {
  for (auto t : *timers) {
    if (t->GetTimeoutValue() == rtcTimeout) {
      return t;
    }
  }
  return nullptr;
}

// Create timer and add to timers list
ClSimTimer* GenerationController::CreateTimer(int64_t timeout) {
  ClSimTimer* timer = new ClSimTimer(timeout);
  InsertTimer(timer);
  return timer;
}

// Add a timer to the timers vector by insertion sorting on increasing timeout value
void GenerationController::InsertTimer(ClSimTimer* timer) {
  bool inserted = false;

  for (auto i = timers->begin(); i != timers->end(); i++) {
    if (timer->GetTimeoutValue() < (*i)->GetTimeoutValue()) { // sorting comparison
      timers->insert(i, timer);
      inserted = true;
      break;
    }
  }

  if (!inserted)
    timers->push_back(timer);
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