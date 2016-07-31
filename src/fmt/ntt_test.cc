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
