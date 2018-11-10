#pragma once

#include <chrono>
#include "base/base.h"

namespace ppi {
namespace base {

class Timer {
  using Clock = std::chrono::system_clock;
  using TimeStamp = Clock::time_point;
  using MilliSec = std::chrono::milliseconds;

 public:
  Timer() { Start(); }

  void Start() { start_time_ = Clock::now(); }

  void Stop() { stop_time_ = Clock::now(); }

  double GetTimeInSec() const;
  int64 GetTimeInMs() const;

 private:
  TimeStamp start_time_;
  TimeStamp stop_time_;
};

}  // namespace base
}  // namespace ppi
