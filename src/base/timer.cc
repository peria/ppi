#include "base/timer.h"

namespace ppi {
namespace base {

double Timer::GetTimeInSec() const {
  return Time::GetTimeInMs() * 1e-3;
}

double Timer::GetTimeInMs() const {
  auto diff = stop_time_ - start_time_;
  return std::chrono::duration_cast<MilliSec>(diff).count();
}

}  // namespace base
}  // namespace ppi
