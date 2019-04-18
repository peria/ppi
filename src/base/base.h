#pragma once

#include <cassert>
#include <cstdint>

namespace ppi {

// C++11 allows us to implement code to take the array size.
// If we pass non-array to this function, compile error will happen.
template<typename T, size_t size>
constexpr size_t array_size(const T (&)[size]) {
  return size;
}

using int64 = std::int64_t;
using int32 = std::int32_t;
using int16 = std::int16_t;
using int8 = std::int8_t;

using uint64 = std::uint64_t;
using uint32 = std::uint32_t;
using uint16 = std::uint16_t;
using uint8 = std::uint8_t;

#if __GNUC__ && !defined(UINT128)
#define UINT128
using uint128 = __uint128_t;
#endif

template<typename T>
T pointer_cast(void* p) {
  return static_cast<T>(p);
}

}  // namespace ppi
