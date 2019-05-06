#pragma once

#include <chrono>

#include "base/base.h"

namespace ppi {
namespace base {

class Timer {
  using Clock = std::chrono::system_clock;
  using TimeStamp = Clock::time_point;

public:
  Timer() = default;

  void start() { start_time_ = Clock::now(); }
  void stop() { stop_time_ = Clock::now(); }

  double getTimeInSec() const;
  int64 getTimeInMs() const;

private:
  TimeStamp start_time_;
  TimeStamp stop_time_;
};

}  // namespace base
}  // namespace ppi

