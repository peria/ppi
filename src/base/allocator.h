#pragma once

#include "base/base.h"

#include <glog/logging.h>

namespace ppi {
namespace base {

class Allocator {
public:
  template<typename T>
  static T* allocate(int64 number) {
    static_assert(sizeof(T) == 8, "Size of T must be 8.");

    T* memory = new uint64[number + 1];
    *memory = number * 8;
    ++memory;
    return memory;
  }

  static void deallocate(void* ptr) {
    uint64* p64 = reinterpret_cast<uint64*>(ptr);
    --p64;
    delete[](p64);
  }

  static int64 getSizeInByte(void* ptr) {
    int64* p64 = reinterpret_cast<int64*>(ptr);
    return p64[-1];
  }
};

}  // namespace base
}  // namespace ppi
