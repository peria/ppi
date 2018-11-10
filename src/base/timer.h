#pragma once

#include <chrono>

namespace ppi {
namespace base {

class Timer {
  using Clock = std::chrono::system_clock;
  using TimeStamp = Clock::time_point;
  using MilliSec = std::chrono::milliseconds;

 public:
  Timer() {
    Start();
  }

  void Start() {
    start_time_ = Clock::now();
  }

  void Stop() {
    stop_time_ = Clock::now();
  }

  double GetTimeInSec() const;
  double GetTimeInMs() const;

 private:
  TimeStamp start_time_;
  TimeStamp stop_time_;
  int64 duration_ms_;
};

}  // namespace base
}  // namespace ppi
