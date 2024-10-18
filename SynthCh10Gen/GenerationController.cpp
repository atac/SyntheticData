#include "GenerationController.h"


GenerationController::GenerationController() {
  simState = nullptr;
  sources = nullptr;
  channels = nullptr;
  timers = nullptr;

  outFileHandle = 0;

  time.srcTime = 0.0;
  time.startSimClockTime = -1.0;
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

// After configuration:
// X init timers
// set time
// write setup channel (change how formatter TMATS is passed to writer)
// write time
// START LOOP


// TODO:
// Need more time conversions

int GenerationController::Init(string configPathname) {
  // TODO: open config file
  int result = TmpCreateConfig();
  if (result != CONTROLLER_OK)
    return result;

  InitTimers();



  // TODO....

  return result;
}

void GenerationController::InitTimers() {
  for (auto timerAction : *timers) {
    timerAction.first->FromNow();
  }
}

int GenerationController::TmpCreateConfig() {
  // from each config item
  string configFileSource = "pathtofile";
  Rate configDataRate(100, RateType::FREQUENCY); // frequency of frames in a packet
  Rate configPacketRate(10, RateType::FREQUENCY); // frequency of packets in a file
  string configOutfile = "pathtofile";
  unsigned int configChanID = 20;

  // init structures
  sources = new std::vector<ClSource_Nav*>();
  channels = new vector<Ch10Channel*>();
  timers = new map<ClSimTimer*, vector<ChannelAction>*>();

  // init simstate object
  simState = new ClSimState();
  simState->clear();


  // create output file
  EnI106Status enStatus = enI106Ch10Open(&outFileHandle, configOutfile.data(), I106_OVERWRITE);
  if (enStatus != I106_OK)
  {
    fprintf(stderr, "Error opening data file : Status = %d\n", enStatus);
    return 1;
  }

  AddIndexWriter();
  AddTimeWriter();

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
  Ch10Channel* channel = new Ch10Channel();
  channels->push_back(channel);
  channel->Init(writer, formatter);

  // create timers with maps to channel/action
  AddTimedChannelAction(channel, configDataRate, ChannelActionType::PUSH);
  AddTimedChannelAction(channel, configPacketRate, ChannelActionType::COMMIT);
  //
  // END FOREACH SOURCE

  return 0;
}

void GenerationController::AddIndexWriter() {
  ClCh10Writer_Index* indexWriter = new ClCh10Writer_Index();
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(indexWriter);
  writer->Init(outFileHandle, 0);
}

void GenerationController::AddTimeWriter() {
  ClCh10Writer_Time* timeWriter = new ClCh10Writer_Time();
  Ch10Writer* writer = dynamic_cast<Ch10Writer*>(timeWriter);
  writer->Init(outFileHandle, 1);
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