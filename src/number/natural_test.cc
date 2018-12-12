#include "number/natural.h"

#include <gtest/gtest.h>

#include <random>

#include "base/base.h"

namespace ppi {
namespace number {

namespace {

void CreateProd(const uint64 a, const uint64 b, const uint64 r, uint64* c) {
  constexpr uint64 kMask = (1ULL << 32) - 1;
  // Compute a product
  uint64 a0 = a & kMask;
  uint64 a1 = a >> 32;
  uint64 b0 = b & kMask;
  uint64 b1 = b >> 32;

  uint64 c0 = a0 * b0;
  uint64 c1 = a1 * b0;
  uint64 c2 = a1 * b1;

  uint64 u = c1 + (a0 * b1);
  if (u < c1) {
    c2 += 1ULL << 32;
  }

  uint64 v = c0 + (u << 32);
  if (v < c0) {
    ++c2;
  }

  // Add reminder
  v += r;
  if (v < r) {
    ++c2;
  }

  // |<-- c[1] -->||<-- c[0] -->|
  // |<--  c2  -->|
  //        |<-- c1,u -->|
  //               |<-- c0,v -->|

  c[0] = v;
  c[1] = c2;
}

}  // namespace

class NaturalForTest : public Natural {
 public:
  using Natural::Gather4;
  using Natural::Split4;
};

TEST(NaturalTest, Div2By1) {
  struct {
    uint64 a[2];
    uint64 b;
    uint64 expect_quot;
    uint64 expect_rem;
  } datas[] = {
      {{0, 1ULL << 63}, (1ULL << 63) + 1, 0xfffffffffffffffe, 2},
      {{0, 1ULL << 62}, (1ULL << 63) - 1, (1ULL << 63) + 1, 1},
      {{0, (1ULL << 63) + (1ULL << 32) - 1},
       (1ULL << 63) + (1ULL << 32),
       0xfffffffffffffffe,
       1ULL << 33},
  };
  for (auto data : datas) {
    uint64 rem;
    uint64 quot = Natural::Div(data.a, data.b, &rem);
    EXPECT_EQ(data.expect_rem, rem)
        << " for 0x" << std::hex << data.a[1] << data.a[0] << " % 0x" << data.b;
    EXPECT_EQ(data.expect_quot, quot)
        << " for 0x" << std::hex << data.a[1] << data.a[0] << " / 0x" << data.b;
  }

  constexpr int64 kTestTimes = 100000;
  std::mt19937 mt(19937);  // Fixed seed
  for (int64 i = 0; i < kTestTimes; ++i) {
    const uint64 b = mt();
    if (b == 0)
      continue;
    const uint64 a = mt();
    const uint64 c = mt() % b;
    uint64 prod[2];
    CreateProd(a, b, c, prod);

    uint64 r;
    uint64 q = Natural::Div(prod, b, &r);
    EXPECT_EQ(a, q);
    EXPECT_EQ(c, r);
  }
}

TEST(NaturalTest, Split) {
  uint64 a[] = {0x1234567890abcdefULL};
  double b[4];

  NaturalForTest::Split4(a, 1, 1, b);
  EXPECT_EQ(static_cast<double>(0xcdef - 0x10000), b[0]);
  EXPECT_EQ(static_cast<double>(0x90ab + 1 - 0x10000), b[1]);
  EXPECT_EQ(static_cast<double>(0x5678 + 1), b[2]);
  EXPECT_EQ(static_cast<double>(0x1234), b[3]);
}

TEST(NaturalTest, Gather) {
  double a[] = {0x4444, 0x3333, 0x2222, 0x1111};
  uint64 b;

  NaturalForTest::Gather4(a, 1, &b);
  EXPECT_EQ(0x1111222233334444ULL, b);
}

}  // namespace number
}  // namespace ppi
