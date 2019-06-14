#include "bbp/bbp.h"

#include <gtest/gtest.h>

#include <vector>

#include "base/base.h"

namespace ppi {

namespace {
const int64 N = Bbp::kLength;
}  // namespace

TEST(BbpTest, Basic) {
  uint64 kExpects[][3] = {
      {0x243f6a8885a308d3u, 0x13198a2e03707344u, 0xa4093822299f31d0u},
      {0x43f6a8885a308d31u, 0x3198a2e03707344au, 0x4093822299f31d00u},
  };

  for (int i = 0; i < array_size(kExpects); ++i) {
    for (auto formula : {Bbp::Formula::kBbp, Bbp::Formula::kBellard}) {
      Bbp bbp(formula);
      std::vector<uint64> a = bbp.compute(i + 1);
      EXPECT_EQ(kExpects[i][0], a[N - 1]);
      EXPECT_EQ(kExpects[i][1], a[N - 2]);
      EXPECT_EQ(kExpects[i][2], a[N - 3]);
    }
  }
}

TEST(BbpTest, BetweenFormulae) {
  const uint64 kPlaces[] = {1, 10, 100, 1000, 10000};

  const Bbp bbp(Bbp::Formula::kBbp);
  const Bbp bellard(Bbp::Formula::kBellard);
  for (uint64 place : kPlaces) {
    std::vector<uint64> a = bbp.compute(place);
    std::vector<uint64> b = bellard.compute(place);
    EXPECT_EQ(a[N - 1], b[N - 1]);
    EXPECT_EQ(a[N - 2], b[N - 2]);
    EXPECT_EQ(a[N - 3], b[N - 3]);
  }
}

TEST(BbpTest, BetweenPlace) {
  const uint64 kPlaces[] = {1, 10, 100, 1000, 10000};
  const int64 kDiff = 3;

  const Bbp bbp(Bbp::Formula::kBellard);
  for (uint64 place : kPlaces) {
    std::vector<uint64> a = bbp.compute(place);
    std::vector<uint64> b = bbp.compute(place + kDiff);
    uint64 carry = 0;
    for (int64 i = 0; i < N; ++i) {
      uint64 c = a[i] >> (64 - 4 * kDiff);
      a[i] = (a[i] << (4 * kDiff)) | carry;
      carry = c;
    }
    EXPECT_EQ(a[N - 1], b[N - 1]);
    EXPECT_EQ(a[N - 2], b[N - 2]);
    EXPECT_EQ(a[N - 3], b[N - 3]);
  }
}

}  // namespace ppi
