#include "StopWatch.h"

StopWatch::StopWatch(bool autostart) : running(false) {
  if (autostart)
    start();
}

void StopWatch::start() {
  startTime = std::chrono::system_clock::now();
  running = true;
}

void StopWatch::stop() {
  stopTime = std::chrono::system_clock::now();
  running = false;
}

int StopWatch::elapsed() {
  if (running)
    stop();
  return std::chrono::duration_cast<std::chrono::milliseconds>(stopTime -
                                                               startTime)
      .count();
}
