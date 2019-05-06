#pragma once

#include "base/base.h"

namespace ppi {
namespace base {

class Allocator {
public:
  template<typename T>
  static T* allocate(int64 size);

  static void deallocate(void* ptr);

private:
  static int64 memory_use_;
  static int64 peak_memory_use_;
};

// static
template<typename T>
T* Allocator::allocate(int64 size) {
  T* ptr = new T[size];
  return ptr;
}

}  // namespace base
}  // namespace ppi
