#include "TimeEstimator.h"

TimeEstimator::TimeEstimator(double estimateWindowSizeInSecs, double updateRate) {
  this->windowTime = estimateWindowSizeInSecs;
  this->secondsPerUpdate = 1 / updateRate;

  currentTime = time(nullptr);
  startTime = currentTime;
  timeOfLastEstimate = currentTime;

  estimatedTime = 0.0;
}

void TimeEstimator::SetProgress(double value) {
  AddSample(value);
  bool windowFull = ClearOutdatedSamples();

  if (windowFull)
    Estimate();
}

string TimeEstimator::GetEstimate() {
  string elapsed = GetElapsedTime();
  string estimate = GetEstimatedTime();

  if (!estimate.empty())
    return elapsed + " (" + estimate + ")";
  else
    return elapsed;
}

void TimeEstimator::Estimate() {
  ProgressSample start = progressWindow.front();
  ProgressSample end = progressWindow.back();

  if (difftime(end.first, timeOfLastEstimate) >= secondsPerUpdate) {
    double remainingProgress = 1.0 - end.second;
    double windowProgress = end.second - start.second;
    double progressRate = windowProgress / windowTime;

    estimatedTime = remainingProgress / progressRate;

    timeOfLastEstimate = end.first;
  }
}

void TimeEstimator::AddSample(double progress) {
  currentTime = time(nullptr);
  ProgressSample sample = ProgressSample(currentTime, progress);
  progressWindow.push_back(sample);
}

bool TimeEstimator::ClearOutdatedSamples() {
  bool windowFull = false;

  time_t cutoff = difftime(currentTime, windowTime);

  while (true) {
    if (progressWindow.empty())
      break;

    double diff = difftime(progressWindow.front().first, cutoff);
    if (diff <= 0.0) {
      progressWindow.pop_front();
      windowFull = true;
    }
    else
      break;
  }

  return windowFull;
}

string TimeEstimator::GetElapsedTime() {
  double newElapsedTime = difftime(currentTime, startTime);

  if (newElapsedTime - lastElapsedTime >= 1.0) {
    elapsedTimeString = BeautifyTime(newElapsedTime);
    lastElapsedTime = newElapsedTime;
  }

  return elapsedTimeString;
}

string TimeEstimator::GetEstimatedTime() {
  if (abs(lastEstimatedTime - estimatedTime) >= 1.0) {
    estimatedTimeString = BeautifyTime(estimatedTime);
    lastEstimatedTime = estimatedTime;
  }

  return estimatedTimeString;
}

string TimeEstimator::BeautifyTime(double seconds) {
  int time = (int)seconds;

  int hours = time / 60 / 60;
  time %= 60 * 60;

  int min = time / 60;
  int sec = time % 60;


  std::ostringstream os;

  if (hours > 0)
    os << hours << ":";

  os << std::setfill('0') << std::setw(2) << min << ":"
    << std::setfill('0') << std::setw(2) << sec;

  return os.str();
}