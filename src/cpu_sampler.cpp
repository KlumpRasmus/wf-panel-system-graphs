#include "cpu_sampler.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

std::optional<CpuCounters> parse_cpu_counters(std::istream& input)
{
    std::string line;
    if (!std::getline(input, line))
    {
        return std::nullopt;
    }

    std::istringstream fields(line);
    std::string name;
    fields >> name;
    if (name != "cpu")
    {
        return std::nullopt;
    }

    std::vector<std::uint64_t> values;
    std::uint64_t value = 0;
    while (fields >> value)
    {
        values.push_back(value);
    }
    if (values.size() < 4)
    {
        return std::nullopt;
    }

    std::uint64_t total = 0;
    for (std::size_t index = 0; index < values.size() && index < 8; ++index)
    {
        total += values[index];
    }
    const std::uint64_t idle = values[3] +
        (values.size() > 4 ? values[4] : 0);
    return CpuCounters {total, idle};
}

std::optional<double> CpuSampler::sample(const std::string& path)
{
    std::ifstream input(path);
    if (!input)
    {
        return std::nullopt;
    }

    const auto counters = parse_cpu_counters(input);
    if (!counters)
    {
        return std::nullopt;
    }
    return sample_counters(*counters);
}

std::optional<double> CpuSampler::sample_counters(const CpuCounters& counters)
{
    if (!have_previous || counters.total <= previous.total ||
        counters.idle < previous.idle)
    {
        previous = counters;
        have_previous = true;
        return 0.0;
    }

    const auto total_delta = counters.total - previous.total;
    const auto idle_delta = counters.idle - previous.idle;
    previous = counters;
    if (total_delta == 0)
    {
        return 0.0;
    }

    const auto busy_delta = total_delta - std::min(total_delta, idle_delta);
    return static_cast<double>(busy_delta) /
        static_cast<double>(total_delta);
}
