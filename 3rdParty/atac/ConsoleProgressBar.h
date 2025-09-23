#include <stdio.h>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "TimeEstimator.h"

using namespace std;

#pragma once
class ConsoleProgressBar
{
private:
  static const unsigned int DEFAULT_SIZE = 50;

  TimeEstimator *time;

public:

  ConsoleProgressBar();
  ConsoleProgressBar(unsigned int size);
  ConsoleProgressBar(double start, double end, unsigned int size = DEFAULT_SIZE);
  ~ConsoleProgressBar();

  bool SetProgress(double value);
  double GetPercent();
  string GetBar();

private:
  unsigned int size;
  double offset;
  double factor;

  double progress;
  double percent;

  void Init(double start, double end, int size);
  bool ValidParameters();
};

