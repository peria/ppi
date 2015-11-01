#include "number/integer.h"

#include <gtest/gtest.h>
#include "base/base.h"

namespace ppi {
namespace number {

TEST(IntegerTest, ErrorInMult) {
  const int n = 64;
  Integer a, b, c;

  a.resize(n);
  b.resize(n);
  for (int i = 0; i < n; ++i) {
    a[i] = b[i] = 0x123456789abcdefULL;
  }

  double err = Integer::Mult(a, b, &c);
  EXPECT_GT(1e-2, err);
}

TEST(IntegerTest, Mult) {
  Integer a, b, c;
  a.push_back(0x123456789abcdef0ULL);
  a.push_back(0x13579bdf2468ace0ULL);
  b.push_back(0x0fedcba987654321ULL);
  b.push_back(0x1234567890123456ULL);
  EXPECT_EQ(2, static_cast<int>(a.size()));
  EXPECT_EQ(2, static_cast<int>(b.size()));

  Integer::Mult(a, b, &c);
  ASSERT_EQ(4, static_cast<int>(c.size()));
  EXPECT_EQ(0x2236d88fe5618cf0ULL, c[0]);
  EXPECT_EQ(0x9cdc93c31e3064c2ULL, c[1]);
  EXPECT_EQ(0xe066ea72819d9ae5ULL, c[2]);
  EXPECT_EQ(0x01601d49e4337009ULL, c[3]);
}

}  // namespace number
}  // namespace ppi
