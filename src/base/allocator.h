#pragma once

#include "base/base.h"

namespace ppi {
namespace base {

//                    v--0Byte  v--8Byte              v--(size+1)*8Byte
// allocated memory : | size    | usable memory area  |
//                              ^-- use this address in pointers
class Allocator {
 public:
  template<typename T>
  static T* Allocate(int64 number) {
    return reinterpret_cast<T*>(AllocateInternal(number));
  }
  static void Deallocate(uint64* ptr);

  static int64 allocated_size() { return allocated_size_; }
  static int64 allocated_size_peak() { return allocated_size_peak_; }
  static int64 allocated_number() { return allocated_number_; }

 private:
  static void* AllocateInternal(int64 number);

  static int64 allocated_size_;
  static int64 allocated_size_peak_;
  static int64 allocated_number_;
};

}  // namespace base
}  // namespace ppi
