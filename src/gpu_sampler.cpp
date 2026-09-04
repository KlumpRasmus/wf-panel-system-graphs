#include "gpu_sampler.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace
{
std::optional<std::size_t> queue_index(const std::string& name)
{
    if (name == "bin" || name == "v3d_bin")
    {
        return 0;
    }
    if (name == "render" || name == "v3d_ren")
    {
        return 1;
    }
    if (name == "tfu" || name == "v3d_tfu")
    {
        return 2;
    }
    if (name == "csd" || name == "v3d_csd")
    {
        return 3;
    }
    if (name == "cache_clean" || name == "v3d_cac")
    {
        return 4;
    }
    return std::nullopt;
}

std::vector<std::string> split_semicolons(const std::string& line)
{
    std::vector<std::string> values;
    std::istringstream fields(line);
    std::string value;
    while (std::getline(fields, value, ';'))
    {
        values.push_back(value);
    }
    return values;
}
} // namespace

std::optional<GpuCounters> parse_gpu_stats(std::istream& input)
{
    GpuCounters counters;
    bool found_queue = false;
    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream fields(line);
        std::string type;
        std::uint64_t timestamp = 0;
        std::uint64_t jobs = 0;
        std::uint64_t runtime = 0;
        if (!(fields >> type >> timestamp >> jobs >> runtime))
        {
            continue;
        }

        const auto index = queue_index(type);
        if (!index)
        {
            continue;
        }
        counters.timestamp = std::max(counters.timestamp, timestamp);
        counters.runtime[*index] = runtime;
        counters.present[*index] = true;
        found_queue = true;
    }

    if (!found_queue || counters.timestamp == 0)
    {
        return std::nullopt;
    }
    return counters;
}

std::optional<GpuCounters> parse_legacy_gpu_usage(std::istream& input)
{
    GpuCounters counters;
    bool found_queue = false;
    std::string line;
    while (std::getline(input, line))
    {
        const auto values = split_semicolons(line);
        if (values.size() >= 2 && values[0] == "timestamp")
        {
            try
            {
                counters.timestamp = std::stoull(values[1]);
            } catch (...)
            {
                return std::nullopt;
            }
            continue;
        }

        if (values.size() < 4)
        {
            continue;
        }
        const auto index = queue_index(values[0]);
        if (!index)
        {
            continue;
        }
        try
        {
            counters.runtime[*index] = std::stoull(values[2]);
        } catch (...)
        {
            return std::nullopt;
        }
        counters.present[*index] = true;
        found_queue = true;
    }

    if (!found_queue || counters.timestamp == 0)
    {
        return std::nullopt;
    }
    return counters;
}

std::optional<double> GpuSampler::sample()
{
    const char *modern_paths[] = {
        "/sys/devices/platform/axi/1002000000.v3d/gpu_stats",
        "/sys/devices/platform/v3dbus/fec00000.v3d/gpu_stats",
    };
    for (const auto *path : modern_paths)
    {
        std::ifstream input(path);
        if (input)
        {
            const auto counters = parse_gpu_stats(input);
            if (counters)
            {
                return sample_counters(*counters);
            }
        }
    }

    const char *legacy_paths[] = {
        "/sys/kernel/debug/dri/0/gpu_usage",
        "/sys/kernel/debug/dri/1/gpu_usage",
    };
    for (const auto *path : legacy_paths)
    {
        std::ifstream input(path);
        if (input)
        {
            const auto counters = parse_legacy_gpu_usage(input);
            if (counters)
            {
                return sample_counters(*counters);
            }
        }
    }
    return std::nullopt;
}

std::optional<double> GpuSampler::sample_counters(const GpuCounters& counters)
{
    if (!have_previous || counters.timestamp <= previous.timestamp)
    {
        previous = counters;
        have_previous = true;
        return 0.0;
    }

    const auto elapsed = counters.timestamp - previous.timestamp;
    double maximum = 0.0;
    for (std::size_t index = 0; index < counters.runtime.size(); ++index)
    {
        if (!counters.present[index] || !previous.present[index] ||
            counters.runtime[index] < previous.runtime[index])
        {
            continue;
        }
        const auto runtime = counters.runtime[index] - previous.runtime[index];
        maximum = std::max(
            maximum, static_cast<double>(runtime) /
                static_cast<double>(elapsed));
    }
    previous = counters;
    return std::clamp(maximum, 0.0, 1.0);
}
