#include "fmt/fmt.h"

#include "base/util.h"

namespace ppi {
namespace fmt {

Config::Config(int64 m) : n(m), log2n(PopCount(m - 1)) {
  DCHECK_EQ(n, 1LL << (log2n));
}

}  // namespace fmt
}  // namespace ppi
