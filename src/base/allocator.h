#pragma once

#include "base/base.h"

namespace ppi {
namespace base {

class Allocator {
 public:
  static uint64* Allocate(int64 number);
  static void Deallocate(uint64* ptr);

  static int64 allocated_size() { return allocated_size_; }
  static int64 allocated_size_peak() { return allocated_size_peak_; }
  static int64 allocated_number() { return allocated_number_; }

 private:
  static int64 allocated_size_;
  static int64 allocated_size_peak_;
  static int64 allocated_number_;
};

}  // namespace base
}  // namespace ppi
