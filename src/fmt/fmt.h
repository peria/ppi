#pragma once

#include "base/base.h"

namespace ppi {
namespace fmt {

class Fmt {
 public:
  enum Type {
    Forward,
    Inverse,
  };
  
  // Convert 
  static void Fmt4(double* da, const int64 n, const Type type);
};

}  // namespace fmt
}  // namespace ppi
