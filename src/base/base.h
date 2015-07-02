#pragma once

#include <cstdint>
#include <cstddef>
#include <cassert>

namespace ppi {

template<typename T, std::size_t size>
constexpr std::size_t array_size(const T (&)[size]) {
  return size;
}

using int8 = std::int8_t;
using int16 = std::int16_t;
using int32 = std::int32_t;
using int64 = std::int64_t;
using uint8 = std::uint8_t;
using uint16 = std::uint16_t;
using uint32 = std::uint32_t;
using uint64 = std::uint64_t;

}  // namespace ppi
