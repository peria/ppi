#include "base/base.h"
#include "fmt/fmt.h"

namespace ppi {

struct Complex;

namespace fmt {

class DFT {
 public:
  DFT(int64 logn);

  void transform(Complex*, Direction);
  void forward(Complex*);
  void backward(Complex*);

 protected:
  const int64 n_;
  const Complex* ws_;
  const Complex* iws_;

 private:
  void radix2(Complex*, int64, int64);
  void radix4(Complex*, int64, int64, const Complex*);
  void iradix4(Complex*, int64, int64, const Complex*);

  // # of radix-k FFT uses. logn_ = log2n_ + 2 * log4n_;
  const int64 log2n_;
  const int64 log4n_;
};

}  // namespace fmt
}  // namespace ppi
