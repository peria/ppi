#include "bbp/bbp_gpu.h"

#include <array>
#include <vector>

// CUDA includes
#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include "number/number.h"
#include "number/natural.h"

namespace ppi {

namespace {

struct GpuTerm {
  int64 a, b, c, d;
};

constexpr int64 kNumBlocks = 256;
constexpr int64 kThreadsPerBlock = 1024;
constexpr int64 kNumThreads = kNumBlocks * kThreadsPerBlock;

__device__
void GpuAdd(uint64* val, const int64 n, const uint64* rval) {
  uint64 carry = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 sum = val[i] + carry;
    carry = (sum < val[i]);  // 0 or 1
    val[i] = sum + rval[i];
    if (val[i] < rval[i])
      carry = 1;
  }
}

__device__
void GpuSubtract(uint64* val, const int64 n, const uint64* rval) {
  uint64 borrow = 0;
  for (int64 i = 0; i < n; ++i) {
    uint64 sum = val[i] - borrow;
    borrow = (sum > val[i]);  // 0 or 1
    val[i] = sum - rval[i];
    if (val[i] > sum)
      borrow = 1;
  }
}

__global__
void computePerThreadFlip(const GpuTerm& term,
                          const int64 n,
                          int64 bit_shift,
                          int64 from,
                          int64 to,
                          uint64* ret) {
  int64 index = threadIdx.x;
  uint64* value = ret + index * n;
  for (int64 i = 0; i < n; ++i) {
    value[i] = 0;
  }

  uint64* q = new uint64[n];
  for (int64 i = from + index; i < to; i += kNumThreads) {
    int64 shift = bit_shift - i * term.b;
    const uint64 mod = term.c * i + term.d;
    uint64 rem = number::Power(2, shift, mod);
    number::Natural::Div(rem, mod, n, q);
    if (i % 2 == 1)
      GpuSubtract(value, n, q);
    else
      GpuAdd(value, n, q);
  }
  delete[] (q);
}

}  // namespace

BbpGpu::BbpGpu(const Formula& formula) : Bbp(formula) {}

void BbpGpu::computeIntegralTerm(const Term& term, int64 bit_shift, int64 from,
                              int64 to, uint64* ret) const {
  GpuTerm gterm { term.a, term.b, term.c, term.d };
  uint64* value_host = new uint64[kLength * kNumThreads];
  uint64* value_device;
  cudaMalloc(reinterpret_cast<void**>(&value_device),
             sizeof(uint64) * kLength * kNumThreads);
  computePerThreadFlip<<<kNumBlocks, kThreadsPerBlock>>>(
      gterm, kLength, bit_shift, from, to, value_host);
  cudaMemcpy(value_host, value_device,
             sizeof(uint64) * kLength * kNumThreads,
             cudaMemcpyDeviceToHost);
  cudaFree(value_device);

  for (int64 i = 0; i < kNumThreads && i < to - from; ++i) {
    Add(ret, &value_host[i * kLength]);
  }
}

}  // namespace ppi
