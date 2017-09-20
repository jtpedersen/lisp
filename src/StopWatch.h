#ifndef STOPWATCH_H_
#define STOPWATCH_H_

#include <chrono>

class StopWatch {
public:
  explicit StopWatch(bool autostart = true);
  void start();
  void stop();
  int elapsed();

private:
  bool running;
  std::chrono::time_point<std::chrono::system_clock> startTime;
  std::chrono::time_point<std::chrono::system_clock> stopTime;
};

#endif /* !STOPWATCH_H_ */
