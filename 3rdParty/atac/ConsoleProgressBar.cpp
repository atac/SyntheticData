#include "ConsoleProgressBar.h"

ConsoleProgressBar::ConsoleProgressBar() {
  Init(0.0, 1.0, 50);
}

ConsoleProgressBar::ConsoleProgressBar(unsigned int size) {
  Init(0.0, 1.0, size);
}

ConsoleProgressBar::ConsoleProgressBar(double start, double end, unsigned int size)
{
  Init(start, end, size);
}

ConsoleProgressBar::~ConsoleProgressBar()
{
}


void ConsoleProgressBar::Init(double start, double end, int size)
{
  offset = start;
  factor = 1 / (end - start);
  progress = 0.0;
  percent = 0.0;

  this->size = size;

  if (!ValidParameters())
    throw exception("Invalid progress bar parameters\n");

  time = new TimeEstimator(5.0, 0.50);
}

bool ConsoleProgressBar::ValidParameters() {
  if (
    size < 2 ||
    size > 1000 ||
    offset < 0.0 ||
    factor < 0.0
    )
    return false;

  return true;
}


bool ConsoleProgressBar::SetProgress(double value)
{
  progress = (value - offset) * factor;
  if (progress > 1.0)
    progress = 1.0;

  percent = progress * 100;

  time->SetProgress(progress);

  return false;
}

double ConsoleProgressBar::GetPercent()
{
  return percent;
}

string ConsoleProgressBar::GetBar()
{
  const static stringstream initial;

  int barProgress = progress * size;
  int barRemaining = size - barProgress;

  stringstream ss;
  ss << "|";
  ss << string(barProgress, '=') << string(barRemaining, '-');
  ss << "| ";
  ss << setw(6) << fixed << setprecision(2) << percent << "%%  ";

  ss << time->GetEstimate() << "  ";

  return ss.str();
}

