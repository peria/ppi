#include "base/time.h"

#include <sys/time.h>

namespace ppi {

double Time::Now() {
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  return tv.tv_sec + tv.tv_usec * 1e-6;
}

}  // namespace ppi
