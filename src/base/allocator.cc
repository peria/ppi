#include "base/allocator.h"

#include <algorithm>

namespace ppi {
namespace base {

uint64* Allocator::Allocate(int64 number) {
  uint64* ptr = new uint64[number + 1];
  *ptr = number;
  return &ptr[1];
}

void Allocator::Deallocate(uint64* ptr) {
  --ptr;
  delete[] ptr;
}

}  // namespace base
}  // namespace ppi
