#pragma once

#include <string>
#include <list>
#include <ctime>

#include <iomanip>
#include <sstream>

using namespace std;

class TimeEstimator
{
private:
  typedef pair<time_t, double> ProgressSample;

  list<ProgressSample> progressWindow;

  double windowTime;
  double secondsPerUpdate;

  time_t startTime;
  time_t currentTime;

  double estimatedTime;
  time_t timeOfLastEstimate;

  string elapsedTimeString = "00:00";
  double lastElapsedTime = 0.0;

  string estimatedTimeString = "";
  double lastEstimatedTime = 0.0;

public:
  TimeEstimator(double estimateWindowTime, double updateRate);

  void SetProgress(double value);

  string GetEstimate();

private:
  void Estimate();
  void AddSample(double progress);
  bool ClearOutdatedSamples();

  string GetElapsedTime();
  string GetEstimatedTime();

  string BeautifyTime(double seconds);
};

