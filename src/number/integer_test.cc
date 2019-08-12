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

  // Each number fits in splitted elements.
  a.push_leading(0x1234ULL);
  b.push_leading(0x1234ULL);
  Integer::Mult(a, b, &c);
  ASSERT_EQ(1, c.size());
  EXPECT_EQ(0x1234ULL * 0x1234ULL, c[0]);

  // The result is in a limb.
  a[0] = 0xabcd1234ULL;
  b[0] = 0x98765432ULL;
  Integer::Mult(a, b, &c);
  ASSERT_EQ(1, static_cast<int>(c.size())) << c;
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
  a.push_leading(0xba686c78678e686bULL);
  a.push_leading(0xac7d868e97d8a076ULL);
  b.push_leading(0x7868d76876b876e8ULL);
  b.push_leading(0x97d6897c7d8976e7ULL);

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

TEST(IntegerTest, MultWithUint64) {
  Integer a(1ULL << 63);
  Integer::Mult(a, 2, &a);
  ASSERT_EQ(2, a.size());
  EXPECT_EQ(0ULL, a[0]);
  EXPECT_EQ(1ULL, a[1]);
}

TEST(IntegerTest, Power) {
  Integer p10;
  Integer::Power(10000000000000000000ULL, 10, &p10);  // 10^19
  Integer p5;
  Integer::Power(19073486328125ULL, 10, &p5);  // 19073486328125 = 5^19
  Integer p10_2;
  Integer::LBitShift(p5, 19 * 10, &p10_2);

  ASSERT_EQ(p10.size(), p10_2.size());
  for (int64 i = 0; i < p10.size(); ++i) {
    EXPECT_EQ(p10[i], p10_2[i]) << "for i = " << i;
  }
}

TEST(IntegerTest, BitShift) {
  Integer a, c;
  a.resize(2);
  a[0] = 0;
  a[1] = 0x10;

  Integer::RBitShift(a, 1, &c);
  EXPECT_EQ(2, c.size());
  EXPECT_EQ(0ULL, c[0]);
  EXPECT_EQ(8ULL, c[1]);

  Integer::RBitShift(a, 5, &c);
  EXPECT_EQ(1, c.size());
  EXPECT_EQ(1ULL << 63, c[0]);

  Integer::LBitShift(a, 1, &c);
  EXPECT_EQ(2, c.size());
  EXPECT_EQ(0ULL, c[0]);
  EXPECT_EQ(32ULL, c[1]);

  Integer::LBitShift(a, 60, &c);
  EXPECT_EQ(3, c.size());
  EXPECT_EQ(0ULL, c[0]);
  EXPECT_EQ(0ULL, c[1]);
  EXPECT_EQ(1ULL, c[2]);
}

}  // namespace number
}  // namespace ppi
