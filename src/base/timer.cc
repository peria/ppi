#include "base/timer.h"

namespace ppi {
namespace base {

double Timer::getTimeInSec() const {
  return getTimeInMs() * 1e-3;
}

int64 Timer::getTimeInMs() const {
  using MilliSec = std::chrono::milliseconds;
  auto diff = stop_time_ - start_time_;
  return std::chrono::duration_cast<MilliSec>(diff).count();
}

}  // namespace base
}  // namespace ppi
