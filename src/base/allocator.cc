#include "base/allocator.h"

#include <algorithm>

#include "base/base.h"

namespace ppi {
namespace base {

// static member variables
int64 Allocator::allocated_size_ = 0;
int64 Allocator::allocated_size_peak_ = 0;
int64 Allocator::allocated_number_ = 0;

void* Allocator::AllocateInternal(int64 number) {
  uint64* ptr = new uint64[number + 1];
  *ptr = number;

#if !defined(BUILD_TYPE_release)
  ++allocated_number_;
  allocated_size_ += (number + 1) * sizeof(int64);
  allocated_size_peak_ = std::max(allocated_size_peak_, allocated_size_);
#endif

  return &ptr[1];
}

void Allocator::Deallocate(void* ptr) {
  uint64* ptr64 = reinterpret_cast<uint64*>(ptr);
  --ptr64;

#if !defined(BUILD_TYPE_release)
  int64 size = *ptr64;
  --allocated_number_;
  allocated_size_ -= (size + 1) * sizeof(uint64);
#endif

  delete[] ptr64;
}

int64 Allocator::GetSize(void* ptr) {
  if (!ptr)
    return 0;
  int64* ptr64 = reinterpret_cast<int64*>(ptr);
  return ptr64[-1];
}

}  // namespace base
}  // namespace ppi
