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

uint64* Allocator::Reallocate(uint64* from_ptr, int64 number) {
  uint64* to_ptr = Allocate(number);
  int64 size = from_ptr[-1];
  std::copy(from_ptr, from_ptr + size, to_ptr);
  return to_ptr;
}

}  // namespace base
}  // namespace ppi
