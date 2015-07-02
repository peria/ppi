#pragma once

#include <cstdint>
#include <cassert>

namespace ppi {

#define UNUSED_VARIABLE(x)  (void)(x)

// C++11 allows us to implement code to take the array size.
// If we pass non-array to this function, compile error will happen.
template<typename T, std::size_t size>
constexpr std::size_t ARRAY_SIZE(const T (&)[size]) { return size; }

using int64 = std::int64_t;
using int32 = std::int32_t;
using int16 = std::int16_t;
using int8  = std::int8_t;

using uint64 = std::uint64_t;
using uint32 = std::uint32_t;
using uint16 = std::uint16_t;
using uint8  = std::uint8_t;

// TODO: Check if build_type is release or not.
#define DCHECK_EQ(a, b) assert((a) == (b))

}  // namespace ppi
