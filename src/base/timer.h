#pragma once

#include <chrono>
#include <utility>

#include "base/base.h"

namespace ppi {

class Timer {
  using Clock = std::chrono::system_clock;
  using TimePoint = Clock::time_point;

 public:
  class Duration : public Clock::duration {
   public:
    explicit Duration(const Clock::duration d);
    double timeInSec() const;
    int64 timeInMsec() const;
  };

  Timer() : start_(Clock::now()), split_(start_) {}

  Duration stop() {
    end_ = Clock::now();
    return time();
  }
  Duration time() const { return Duration(end_ - start_); }

  double timeInSec() const { return time().timeInSec(); }
  int64 timeInMsec() const { return time().timeInMsec(); }

  Duration split() {
    TimePoint now = Clock::now();
    Duration d(now - split_);
    split_ = now;
    return d;
  }

 private:
  const TimePoint start_;
  TimePoint split_;
  TimePoint end_;
};

}  // namespace ppi
