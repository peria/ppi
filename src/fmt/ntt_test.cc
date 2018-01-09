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

TEST_F(NttTest, DISABLED_Transfer) {
  const int64 kNumElements = 1 << 2;
  const int64 kElementSize = kNumElements + 1;
  const int64 kTotalSize = kNumElements * kElementSize;
  uint64 value[kTotalSize];
  for (int64 i = 0; i < kTotalSize; ++i) {
    value[i] = ((i + 1) % kElementSize == 0) ? 0 : (i + 1);
  }
  Ntt::Transfer(Direction::Forward, kNumElements, value);
  Ntt::Transfer(Direction::Backward, kNumElements, value);
  for (int64 i = 0; i < kTotalSize; ++i) {
    uint64 expected = ((i + 1) % kElementSize == 0) ? 0 : (i + 1);
    EXPECT_EQ(expected, value[i])
        << "[" << i << "] " << std::hex << expected << " - " << value[i];
  }
}

TEST_F(NttTest, ShiftLeftWords) {
  const int64 N = 4;
  const int64 kSize = N + 1;
  const uint64 kFullBits = 0xffffffffffffffff;

  struct TestData {
    uint64 value[kSize];
    int64 shift;
    uint64 expect[kSize];
  } datas[] = {
    {{1, 0, 0, 0, 0}, 2, {0, 0, 1, 0, 0}},
    {{0, 1, 0, 0, 0}, 3, {0, 0, 0, 0, 1}},
    {{0, 0, 0, 0, 1}, 1, {1, kFullBits, kFullBits, kFullBits, 0}},
    {{0, 0, 0, 0, 1}, 6, {0, 0, 1, 0, 0}},
    {{1, 0, 0, 0, 0}, 5, {1, kFullBits, kFullBits, kFullBits, 0}},
  };

  for (auto& data : datas) {
    uint64 output[kSize];
    ShiftLeftWords(data.value, data.shift, N, output);
    for (int64 i = 0; i < kSize; ++i) {
      EXPECT_EQ(data.expect[i], output[i])
          << "[" << i << "] w = " << data.shift << " : "
          << std::hex << data.expect[i] << " - " << output[i];
    }
  }
}

TEST_F(NttTest, ShiftRightBits) {
  const int64 N = 2;
  const int64 kSize = N + 1;

  struct TestData {
    uint64 value[kSize];
    int64 shift;
    uint64 expect[kSize];
  } datas[] = {
    { {1 << 4, 0, 0}, 2, {1 << 2, 0, 0} },
    { {1, 0, 0}, 2, {1, 0xc000000000000000ULL, 0} },
    { {1, 0, 0}, 3, {1, 0xe000000000000000ULL, 0} },
    { {0xffffffffffffffff, 0, 0}, 2, {1ULL << 62, 1ULL << 62, 0} },
    { {0x7fffffffffffffff, 0, 0}, 2, {1ULL << 61, 1ULL << 62, 0} },
  };

  for (auto& data : datas) {
    uint64 output[kSize];
    ShiftRightBits(data.value, data.shift, N, output);
    EXPECT_EQ(data.expect[0], output[0])
        << std::hex << data.expect[0] << " - " << output[0];
    EXPECT_EQ(data.expect[1], output[1])
        << std::hex << data.expect[1] << " - " << output[1];
  }
}

}  // namespace fmt
}  // namespace ppi
