#pragma once

#include <cstdint>
#include <istream>
#include <optional>
#include <string>

struct CpuCounters
{
    std::uint64_t total = 0;
    std::uint64_t idle = 0;
};

std::optional<CpuCounters> parse_cpu_counters(std::istream& input);

class CpuSampler
{
  public:
    std::optional<double> sample(const std::string& path = "/proc/stat");
    std::optional<double> sample_counters(const CpuCounters& counters);

  private:
    CpuCounters previous;
    bool have_previous = false;
};
