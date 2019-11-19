#include "number/internal.h"

#include <cstdio>
#include <cinttypes>

#include "number/float.h"

namespace ppi {
namespace internal {

std::ostream& operator<<(std::ostream& os, const Natural& a) {
  if (a.size() == 0)
    return os << "0";

  static char buffer[25];
  std::sprintf(buffer, "%" PRIu64, a[a.size() - 1]);
  os << buffer;
  for (int64 i = a.size() - 2; i >= 0; --i) {
    std::sprintf(buffer, "%016" PRIu64, a[i]);
    os << buffer;
  }
  return os;
}

std::ostream& operator<<(std::ostream& os, const Float& f) {
  if (f.size() == 0)
    return os << "0";

  if (f.sign() < 0)
    os << "-";
  os << "0.";
  static char buffer[25];
  for (int64 i = f.size() - 1; i >= 0; --i) {
    std::sprintf(buffer, "%016" PRIu64, f[i]);
    os << buffer;
  }
  os << "e" << (f.exponent() + f.size());

  return os;
}

}  // namespace internal
}  // namespace ppi
