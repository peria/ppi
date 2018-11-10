#include "base/timer.h"

namespace ppi {
namespace base {

double Timer::GetTimeInSec() const {
  return GetTimeInMs() * 1e-3;
}

int64 Timer::GetTimeInMs() const {
  auto diff = stop_time_ - start_time_;
  return std::chrono::duration_cast<MilliSec>(diff).count();
}

}  // namespace base
}  // namespace ppi
