#pragma once

#include <array>
#include <cstdint>
#include <istream>
#include <optional>

struct GpuCounters
{
    std::uint64_t timestamp = 0;
    std::array<std::uint64_t, 5> runtime {};
    std::array<bool, 5> present {};
};

std::optional<GpuCounters> parse_gpu_stats(std::istream& input);
std::optional<GpuCounters> parse_legacy_gpu_usage(std::istream& input);

class GpuSampler
{
  public:
    std::optional<double> sample();
    std::optional<double> sample_counters(const GpuCounters& counters);

  private:
    GpuCounters previous;
    bool have_previous = false;
};
