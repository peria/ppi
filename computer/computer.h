#pragma once

#include <memory>

#include "base/base.h"
#include "number/real.h"

namespace ppi {

// Computer class is an abstract environment to compute a constant number.
class Computer {
 public:
  enum class TargetConstant {
    kPi,
  };
  enum class Formula {
    kChudnovsky,
  };
  enum class WorkType {
    kFromScratch,
    kResume,
    kBorg,
  };
  struct Parameter {
    void ParseInitFile();
    void ParseCommandLine(int, char**);
    bool IsValid() const;

    int64 decimal_digits;
    TargetConstant target = TargetConstant::kPi;
    Formula formula = Formula::kChudnovsky;
    int64 num_threads_per_process;
    WorkType work_type = WorkType::kFromScratch;
  };

  static std::unique_ptr<Computer> Create(const Parameter& parameter);
  virtual ~Computer() = default;

  void Run();
  virtual Real&& Compute() = 0;

  virtual const char* formula() const = 0;
  virtual const char* algorithm() const = 0;

 protected:
  Computer(const Parameter& parameter) : parameter_(parameter) {}

  const Parameter& parameter_;
};

}  // namespace ppi
