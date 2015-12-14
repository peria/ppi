#pragma once

#include "base/base.h"

namespace ppi {
namespace base {

class Allocator {
 public:
  static uint64* Allocate(int64 number);
  static void Deallocate(uint64* ptr);
  static uint64* Reallocate(uint64* from_ptr, int64 number);
};

}  // namespace base
}  // namespace ppi
