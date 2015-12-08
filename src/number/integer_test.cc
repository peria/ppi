#include "number/integer.h"

#include <gtest/gtest.h>
#include "base/base.h"

namespace ppi {
namespace number {

class IntegerForTest : public Integer {
 public:
  using Integer::Split4In8;
  using Integer::Gather4In8;
};

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

TEST(IntegerTest, SplitAndGather) {
  Integer a;
  double d[4 * 2];
  a.push_back(0x1234567890abcdefULL);

  // Integer -> double[4][2]
  IntegerForTest::Split4In8(a, 1, d);
  EXPECT_EQ(static_cast<double>(0xcdef), d[0]);
  EXPECT_EQ(0.0, d[1]);
  EXPECT_EQ(static_cast<double>(0x90ab), d[2]);
  EXPECT_EQ(0.0, d[3]);
  EXPECT_EQ(static_cast<double>(0x5678), d[4]);
  EXPECT_EQ(0.0, d[5]);
  EXPECT_EQ(static_cast<double>(0x1234), d[6]);
  EXPECT_EQ(0.0, d[7]);

  // double[4][2] -> Integer
  d[1] = 3141.0;
  Integer b;
  b.resize(2);
  IntegerForTest::Gather4In8(d, &b);
  EXPECT_EQ(0x1234567890abcdefULL, b[0]);
  EXPECT_EQ(3141ULL, b[1]);
}

TEST(IntegerTest, Mult) {
  Integer a, b, c;

  // Each number fits in splitted elements.
  a.push_back(0x1234ULL);
  b.push_back(0x1234ULL);
  Integer::Mult(a, b, &c);
  ASSERT_EQ(1, static_cast<int>(c.size()));
  EXPECT_EQ(0x1234ULL * 0x1234ULL, c[0]);

  // The result is in a limb.
  a[0] = 0xabcd1234ULL;
  b[0] = 0x98765432ULL;
  Integer::Mult(a, b, &c);
  ASSERT_EQ(1, static_cast<int>(c.size()));
  EXPECT_EQ(0xabcd1234ULL * 0x98765432ULL, c[0]);

  // The result is not in a limb.
  a[0] = 0x1000100010001ULL;
  b[0] = 0x1000100010001ULL;
  Integer::Mult(a, b, &c);
  ASSERT_EQ(2, static_cast<int>(c.size()));
  EXPECT_EQ(0x4000300020001ULL, c[0]);
  EXPECT_EQ(0x0000100020003ULL, c[1]);
}

TEST(IntegerTest, LongMult) {
  Integer a, b, c;
  a.push_back(0xba686c78678e686bULL);
  a.push_back(0xac7d868e97d8a076ULL);
  b.push_back(0x7868d76876b876e8ULL);
  b.push_back(0x97d6897c7d8976e7ULL);

  Integer::Mult(a, b, &c);
  ASSERT_EQ(4, static_cast<int>(c.size()));
  EXPECT_EQ(0xc5ec27cddc17f2f8ULL, c[0]);
  EXPECT_EQ(0x072b6856aa68cfceULL, c[1]);
  EXPECT_EQ(0x1ae057afe5749b51ULL, c[2]);
  EXPECT_EQ(0x664e97efa5291c0fULL, c[3]);
}

}  // namespace number
}  // namespace ppi
