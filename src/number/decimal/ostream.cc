#include "number/internal.h"

#include <cstdio>
#include <cinttypes>

namespace ppi {
namespace internal {

std::ostream& operator<<(std::ostream& os, const Natural& a) {
  if (a.size() == 0)
    return os << "0";

  static char buffer[25];
  std::sprintf(buffer, "%" PRIu64, a[a.size() - 1]);
  os << buffer;
  for (int64 i = a.size() - 2; i >= 0; --i) {
    std::sprintf(buffer, "%016" PRIu64, a[a.size() - 1]);
    os << buffer;
  }
  return os;
}

}  // namespace internal
}  // namespace ppi
