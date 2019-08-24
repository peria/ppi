#pragma once

#include "base/base.h"

namespace ppi {
namespace base {

class Allocator {
public:
  template<typename T>
  static T* allocate(int64 number) {
    static_assert(sizeof(T) == 8, "Size of T must be 8.");

    T* memory = new uint64[number + 1];
    return memory + 1;
  }
};

}  // namespace base
}  // namespace ppi
