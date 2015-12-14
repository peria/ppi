#pragma once

#include "base/base.h"

namespace ppi {
namespace base {

class Allocator {
 public:
  static uint64* Allocate(int64 number);
  static void Deallocate(uint64* ptr);
};

}  // namespace base
}  // namespace ppi
