#pragma once

#include <memory>

#include "base/base.h"
#include "base/complex.h"

namespace ppi {
namespace fmt {

class Fmt {
public:
  enum class Direction {
    Forward,
    Backward,
  };

  struct Config {
    explicit Config(int64 n);
    ~Config();

    int64 n;
    int64 log2n;
    Complex* table;
  };
};

}  // namespace fmt
}  // namespace ppi
