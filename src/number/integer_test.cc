#include "number/integer.h"

#include <gtest/gtest.h>
#include "base/base.h"

namespace ppi {
namespace number {

class IntegerForTest : public Integer {
 public:
  using Integer::Split4Round;
  using Integer::Gather4Round;
  using Integer::Split4Serial;
  using Integer::Gather4Serial;
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

TEST(IntegerTest, SplitAndGatherSerial) {
  double c[] = {0x4444, 0x3333, 0x2222, 0x1111};

  // Complex[2] -> Integer
  Integer a;
  a.resize(1);
  IntegerForTest::Gather4Serial(c, &a);
  EXPECT_EQ(0x1111222233334444ULL, a[0]);

  // Integer -> Complex[2]
  Integer b;
  b.push_back(0x1234567890abcdefULL);
  IntegerForTest::Split4Serial(b, 1, c);
  EXPECT_EQ(static_cast<double>(0xcdef), c[0]);
  EXPECT_EQ(static_cast<double>(0x90ab), c[1]);
  EXPECT_EQ(static_cast<double>(0x5678), c[2]);
  EXPECT_EQ(static_cast<double>(0x1234), c[3]);

  // Nega-cyclic
  b.push_back(0xfedcba0987654321ULL);
  IntegerForTest::Split4Serial(b, 1, c);
  EXPECT_EQ(static_cast<double>(0xcdef - 0x4321), c[0]);
  EXPECT_EQ(static_cast<double>(0x90ab - 0x8765), c[1]);
  EXPECT_EQ(static_cast<double>(0x5678 - 0xba09), c[2]);
  EXPECT_EQ(static_cast<double>(0x1234 - 0xfedc), c[3]);
}

TEST(IntegerTest, Mult) {
  Integer a, b, c;

  // Each number fits in splitted elements.
  a.push_back(0x1234ULL);
  b.push_back(0x1234ULL);
  Integer::Mult(a, b, &c);
  ASSERT_EQ(1, c.size());
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

TEST(IntegerTest, Add) {
  Integer a(1ULL << 63);
  EXPECT_EQ(1, a.size());
  Integer::Add(a, a, &a);
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0ULL, a[0]);
  EXPECT_EQ(1ULL, a[1]);
}

TEST(IntegerTest, MultWithUint32) {
  Integer a(1ULL << 63);
  Integer::Mult(a, 2, &a);
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0ULL, a[0]);
  EXPECT_EQ(1ULL, a[1]);
}

// Enable after implement Mult(, uint64,)
/*
TEST(IntegerTest, DISABLD_MultWithUint64) {
  Integer a(1ULL << 32);
  Integer::Mult(a, 1ULL << 32, &a);
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0ULL, a[0]);
  EXPECT_EQ(1ULL, a[1]);
}
*/

}  // namespace number
}  // namespace ppi
