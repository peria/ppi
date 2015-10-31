#include "number/integer.h"

#include <gtest/gtest.h>
#include "base/base.h"

namespace ppi {
namespace number {

TEST(IntegerTest, ErrorInMult) {
  const int n = 64;
  uint64 storage[4 * n];
  Integer a, b, c;
  for (int i = 0; i < 2 * n; ++i)
    storage[i] = 0x123456789abcdefULL;

  a.resize(n);
  b.resize(n);
  c.resize(2 * n);

  double err = Integer::Mult(a, b, &c);
  EXPECT_GT(1e-2, err);
}

}  // namespace number
}  // namespace ppi
