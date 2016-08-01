#include "fmt/ntt.h"

#include <gtest/gtest.h>

namespace ppi {
namespace fmt {

class TestableNtt : public Ntt {
 public:
  using Ntt::ShiftLeftWords;
  using Ntt::ShiftRightBits;
};

class NttTest : public testing::Test {
 public:
  static void ShiftLeftWords(const uint64* a, const int64 w, const int64 n, uint64* b) {
    TestableNtt::ShiftLeftWords(a, w, n, b);
  }

  static void ShiftRightBits(const uint64* a, const int64 k, const int64 n, uint64* b) {
    TestableNtt::ShiftRightBits(a, k, n, b);
  }
};

TEST_F(NttTest, ShiftLeftWords) {
  const int kSize = 4;
  const uint64 kFullBits = 0xffffffffffffffff;

  struct TestData {
    uint64 value[kSize];
    int64 shift;
    uint64 expect[kSize];
  } datas[] = {
    { {1, 0, 0, 0}, 2, {0, 0, 1, 0} },
    { {1, 0, 0, 0}, 4, {kFullBits, kFullBits, kFullBits, kFullBits} },
  };

  for (auto& data : datas) {
    uint64 output[kSize];
    ShiftLeftWords(data.value, data.shift, kSize, output);
    for (int64 i = 0; i < kSize; ++i) {
      EXPECT_EQ(data.expect[i], output[i])
          << "[" << i << "]: "
          << std::hex << data.expect[i] << " - " << output[i];
    }
  }
}

TEST_F(NttTest, ShiftRightBits) {
  const int kSize = 2;
  struct TestData {
    uint64 value[kSize];
    int64 shift;
    uint64 expect[kSize];
  } datas[] = {
    { {1 << 4, 0}, 2, {1 << 2, 0} },
    { {1, 0}, 2, {1, 0xc000000000000000ULL} },
    { {1, 0}, 3, {1, 0xe000000000000000ULL} },
    { {0xffffffffffffffff, 0}, 2, {1ULL << 62, 1ULL << 62} },
    { {0x7fffffffffffffff, 0}, 2, {1ULL << 61, 1ULL << 62} },
  };

  for (auto& data : datas) {
    uint64 output[kSize];
    ShiftRightBits(data.value, data.shift, kSize, output);
    EXPECT_EQ(data.expect[0], output[0])
        << std::hex << data.expect[0] << " - " << output[0];
    EXPECT_EQ(data.expect[1], output[1])
        << std::hex << data.expect[1] << " - " << output[1];
  }
}

}  // namespace fmt
}  // namespace ppi
