#include "base/util.h"

namespace ppi {
namespace {

const uint64 kBitMask0 = 0x5555555555555555ULL;
const uint64 kBitMask1 = 0x3333333333333333ULL;
const uint64 kBitMask2 = 0x0f0f0f0f0f0f0f0fULL;
const uint64 kBitMult = 0x0101010101010101ULL;

}  // namespace

int64 PopCount(uint64 n) {
  n = (n & kBitMask0) + ((n >> 1) & kBitMask0);
  n = (n & kBitMask1) + ((n >> 2) & kBitMask1);
  n = (n  + (n >> 4)) & kBitMask2;
  return (n * kBitMult) >> (64 - 8);
}

}  // namespace ppi
