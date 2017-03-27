#pragma once

#include <memory>

#include "base/base.h"

namespace ppi {
namespace fmt {

enum class Direction {
  Forward,
  Backward,
};

struct Config {
 public:
  explicit Config(int64 n);
  ~Config();

  int64 n;
  int64 log2n;
  Complex* table;
};

}  // namespace fmt
}  // namespace ppi
