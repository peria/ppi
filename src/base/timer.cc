#include "base/timer.h"

namespace ppi {

Timer::Duration::Duration(const Clock::duration d)
    : Clock::duration(d) {}

double Timer::Duration::timeInSec() const {
  return timeInMsec() * 1e-3;
}

int64 Timer::Duration::timeInMsec() const {
  using namespace std::chrono;
  return duration_cast<milliseconds>(*this).count();
}

}  // namespace ppi
