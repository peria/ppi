#include "base/allocator.h"

namespace ppi {
namespace base {

int64 Allocator::memory_use_ = 0;
int64 Allocator::peak_memory_use_ = 0;

void Allocator::deallocate(void* ptr) {
  uint8* u8ptr = reinterpret_cast<uint8*>(ptr);
  delete u8ptr;
}

}  // namespace base
}  // namespace ppi
