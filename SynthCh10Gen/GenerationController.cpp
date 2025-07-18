#include "GenerationController.h"


// TODO:
// Need more time conversions
// Decide if we need a case for poll and packet rate being "on demand"
// Should we find a way to combine sources for channels that use the same source?


GenerationController::GenerationController() {
  simState = nullptr;
  sources = nullptr;
  channels = nullptr;
  timers = nullptr;

  i106OutFileHandle = -1;

  time.srcTime = 0.0;
  time.startSimClockTime = -1.0;

  programName = "";
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

  if (config.programName.size() > 0)
    this->programName = config.programName;

  if (InitOutputFile(config.outputDirectory, config.outputFilename) != 0)
    return ControllerStatus::OPEN_OUTPUT_FILE_FAILED;

  ClCh10Writer_Time* timeWriter = AddTimeChannel();
  AddIndexChannel(timeWriter);

  // Initialize each channel
  for (int i = 0; i < config.channels.size(); i++) {
    if (AddDataChannel(config.channels[i]) != ControllerStatus::OK)
      return ControllerStatus::OPEN_SOURCE_FILE_FAILED;
  }

  timeWriter->SetRelTime(ClSimTimer::lSimClockTicks, this->time.startSimClockTime);

  return ControllerStatus::OK;
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

int GenerationController::InitOutputFile(string directory, string filename) {
  string pathname = directory + "/" + filename;

  // create output file
  EnI106Status enStatus = enI106Ch10Open(&i106OutFileHandle, pathname.data(), I106_OVERWRITE);
  if (enStatus != I106_OK)
  {
    fprintf(stderr, "Error opening output file : Status = %d\n", enStatus);
    return 1;
  }

  return 0;
}

ControllerStatus GenerationController::AddDataChannel(ConfigChannel config) {  // FOREACH SOURCE

  ClSource_Nav* source = CreateSource(config.dataSource, config.id);

  if (source == nullptr) {
    fprintf(stderr, "Failed to open source");
    return ControllerStatus::OPEN_SOURCE_FILE_FAILED;
  }

  // get start time from source
  time.startSimClockTime = source->fStartTime;

  Rate framerate = config.pollRate;
  framerate.ConvertUnits(RateUnit::HERTZ);

  // create formatter/writer pair
  Ch10Formatter* formatter = nullptr;
  Ch10Writer* writer = nullptr;

  switch (config.type) {

  case Ch10Channel::ChannelType::PCM: 
  {
    Ch10Formatter_PCM* formatPcm = CreatePcmFormatter(config.format, framerate, source);
    ClCh10Writer_PCM* writerPcm = new ClCh10Writer_PCM();
    writerPcm->Init(i106OutFileHandle, config.id, formatPcm);

    formatter = dynamic_cast<Ch10Formatter*>(formatPcm);
    writer = dynamic_cast<Ch10Writer*>(writerPcm);
    break;
  }
  
  case Ch10Channel::ChannelType::MS1553:
  {
    Ch10Formatter_1553* format1553 = Create1553Formatter(config.format, source);
    ClCh10Writer_1553* writer1553 = new ClCh10Writer_1553();
    writer1553->Init(i106OutFileHandle, config.id, format1553);

    formatter = dynamic_cast<Ch10Formatter*>(format1553);
    writer = dynamic_cast<Ch10Writer*>(writer1553);
    break;
  }

  case Ch10Channel::ChannelType::A429:
  {
    Ch10Formatter_ARINC429* formatA429 = CreateA429Formatter(config.format, source, 0, 1);
    ClCh10Writer_A429* writerA429 = new ClCh10Writer_A429();
    writerA429->Init(i106OutFileHandle, config.id, formatA429);

    formatter = dynamic_cast<Ch10Formatter*>(formatA429);
    writer = dynamic_cast<Ch10Writer*>(writerA429);
    break;
  }

  case Ch10Channel::ChannelType::VIDEO:
  {
    Ch10Formatter_Video* formatVideo = CreateVideoFormatter(config.format, source);
    Ch10Writer_Video* writerVideo = new Ch10Writer_Video();
    writerVideo->Init(i106OutFileHandle, config.id, formatVideo);

    formatter = dynamic_cast<Ch10Formatter*>(formatVideo);
    writer = dynamic_cast<Ch10Writer*>(writerVideo);
    break;
  }

  default:
    break;
  }

  assert(formatter != nullptr);
  assert(writer != nullptr);

  // create channel passing formatter/writer
  Ch10Channel* channel = CreateChannel(writer, formatter, config.type, config.name);

  // create timers for channel actions
  AddTimedChannelAction(channel, config.pollRate, ChannelActionType::PUSH);
  AddTimedChannelAction(channel, config.packetRate, ChannelActionType::COMMIT);

  return ControllerStatus::OK;
}

void GenerationController::AddIndexChannel(ClCh10Writer_Time* timeWriter) {
  // TODO: consider making these user-configurable
  Rate indexAppendRate(1000, RateUnit::TIME_MS);
  Rate nodeCommitRate(6000, RateUnit::TIME_MS);
  int indexNodesPerRoot = 10;

  Ch10Format_Index* formatter = new Ch10Format_Index();
  formatter->Init(&(timeWriter->suWritePktTimeF1.suCh10Header));

  ClCh10Writer_Index* indexWriter = new ClCh10Writer_Index();
  indexWriter->Init(i106OutFileHandle, 0, 10, formatter);
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(indexWriter);

  Ch10Channel::ChannelType type = Ch10Channel::ChannelType::INDEX;

  Ch10Channel* channel = CreateChannel(writer, formatter, type);

  AddTimedChannelAction(channel, indexAppendRate, ChannelActionType::PUSH);
  AddTimedChannelAction(channel, nodeCommitRate, ChannelActionType::COMMIT);
}

ClCh10Writer_Time* GenerationController::AddTimeChannel() {
  Ch10Format_Time* formatter = new Ch10Format_Time();

  ClCh10Writer_Time* timeWriter = new ClCh10Writer_Time();
  timeWriter->Init(i106OutFileHandle, 1, formatter);
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(timeWriter);

  Ch10Channel::ChannelType type = Ch10Channel::ChannelType::TIME;

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
  case Ch10Channel::ChannelType::VIDEO:
    prefix = "Video";
    break;
  case Ch10Channel::ChannelType::TIME:
    prefix = "Time";
    break;
  case Ch10Channel::ChannelType::INDEX:
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

ClSource_Nav* GenerationController::CreateSource(ConfigDataSource config, int channelID) 
{
  ClSource_Nav* src = nullptr;

  string sourcePrefix = "src" + to_string(channelID);

  switch (config.type) {

  case SourceFileType::CSV:
  {
    ClSource_CsvTxt* csvSrc = new ClSource_CsvTxt(simState, sourcePrefix);
    src = dynamic_cast<ClSource_Nav*>(csvSrc);
    break;
  }

  case SourceFileType::SQLITE:
  {
    ClSource_SQLiteDB* dbSrc = new ClSource_SQLiteDB(simState, sourcePrefix);
    dbSrc->Config(config.properties["tableName"]);
    src = dynamic_cast<ClSource_Nav*>(dbSrc);
    break;
  }
         
  case SourceFileType::INVALID:
  default:
    break;

  }

  if (src != nullptr) {
    src->SetMapping(config.mapping);
    if (!src->Open(config.pathname)) {
      delete src;
      src = nullptr;
    }
    else
      sources->push_back(src);
  }

  return src;
}

Ch10Formatter_PCM* GenerationController::CreatePcmFormatter(Ch10Channel::ChannelDataFormat format, Rate framerate, ClSource_Nav* src) {
  Ch10Formatter_PCM* formatter = nullptr;

  switch (format) {

  case Ch10Channel::ChannelDataFormat::UNFORMATTED:
  {
    // for unformatted channels, get field labels and types directly from source
    ClCh10Format_PCM_CSV* formatCsv =
      new ClCh10Format_PCM_CSV(
        framerate.value,
        src->DataLabels,
        src->DataTypes
      );
    formatter = dynamic_cast<Ch10Formatter_PCM*>(formatCsv);
    break;
  }

  case Ch10Channel::ChannelDataFormat::SYNTHFORMAT1:
  {
    // for formats with fixed fields, pass in a prefix to match the correct source fields
    ClCh10Format_PCM_SynthFmt1* formatSF1 =
      new ClCh10Format_PCM_SynthFmt1(framerate.value, src->sPrefix);
    formatter = dynamic_cast<Ch10Formatter_PCM*>(formatSF1);
    break;
  }

  case Ch10Channel::ChannelDataFormat::CUSTOM:
    break;

  default:
    break;
  }

  return formatter;
}

Ch10Formatter_1553* GenerationController::Create1553Formatter(Ch10Channel::ChannelDataFormat format, ClSource_Nav* src)
{
  Ch10Formatter_1553* formatter = nullptr;

  switch (format) {

  case Ch10Channel::ChannelDataFormat::UNFORMATTED:
    break;

  case Ch10Channel::ChannelDataFormat::SYNTHFORMAT1:
  {
    ClCh10Format_1553_Nav* formatSF1 = new ClCh10Format_1553_Nav(6, 1, 29, 32, src->sPrefix);
    formatter = dynamic_cast<Ch10Formatter_1553*>(formatSF1);
    break;
  }

  case Ch10Channel::ChannelDataFormat::CUSTOM:
    break;

  default:
    break;
  }

  return formatter;
}

Ch10Formatter_ARINC429* GenerationController::CreateA429Formatter(Ch10Channel::ChannelDataFormat format, ClSource_Nav* src, int busSpeed, int engineNumber)
{
  Ch10Formatter_ARINC429* formatter = nullptr;

  switch (format) {

  case Ch10Channel::ChannelDataFormat::UNFORMATTED:
    break;

  case Ch10Channel::ChannelDataFormat::SYNTHFORMAT1:
  {
    Ch10Format_ARINC429_AR100* formatA429 = new Ch10Format_ARINC429_AR100(0, busSpeed, engineNumber, src->sPrefix);
    formatter = dynamic_cast<Ch10Formatter_ARINC429*>(formatA429);
    break;
  }

  case Ch10Channel::ChannelDataFormat::CUSTOM:
    break;

  default:
    break;
  }

  return formatter;
}

Ch10Formatter_Video* GenerationController::CreateVideoFormatter(Ch10Channel::ChannelDataFormat format, ClSource_Nav* src)
{
  Ch10Formatter_Video* formatter = nullptr;

  switch (format) {

  case Ch10Channel::ChannelDataFormat::UNFORMATTED:
    break;

  case Ch10Channel::ChannelDataFormat::SYNTHFORMAT1:
    break;

  case Ch10Channel::ChannelDataFormat::CUSTOM:
    break;

  default:
  {
    Ch10Format_Video* formatVideo = new Ch10Format_Video(src->DataLabels, src->DataTypes);
    formatter = dynamic_cast<Ch10Formatter_Video*>(formatVideo);
    break;
  }
  }

  return formatter;
}