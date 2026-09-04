#include "memory_stats.hpp"

#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <string>

std::uint64_t MemoryStats::used_kib() const
{
    return total_kib - std::min(total_kib, available_kib);
}

double MemoryStats::used_fraction() const
{
    if (total_kib == 0)
    {
        return 0.0;
    }

    return static_cast<double>(used_kib()) /
        static_cast<double>(total_kib);
}

std::optional<MemoryStats> parse_memory_stats(std::istream& input)
{
    std::uint64_t total = 0;
    std::uint64_t available = 0;
    std::uint64_t free = 0;
    std::uint64_t buffers = 0;
    std::uint64_t cached = 0;
    std::uint64_t reclaimable = 0;
    std::uint64_t shared = 0;
    bool have_available = false;

    std::string line;
    while (std::getline(input, line))
    {
        std::istringstream fields(line);
        std::string key;
        std::uint64_t value = 0;
        if (!(fields >> key >> value))
        {
            continue;
        }

        if (!key.empty() && key.back() == ':')
        {
            key.pop_back();
        }

        if (key == "MemTotal")
        {
            total = value;
        } else if (key == "MemAvailable")
        {
            available = value;
            have_available = true;
        } else if (key == "MemFree")
        {
            free = value;
        } else if (key == "Buffers")
        {
            buffers = value;
        } else if (key == "Cached")
        {
            cached = value;
        } else if (key == "SReclaimable")
        {
            reclaimable = value;
        } else if (key == "Shmem")
        {
            shared = value;
        }
    }

    if (total == 0)
    {
        return std::nullopt;
    }

    if (!have_available)
    {
        const auto cache_without_shared = cached > shared ? cached - shared : 0;
        const auto max = std::numeric_limits<std::uint64_t>::max();
        available = free;
        for (const auto component : {buffers, cache_without_shared, reclaimable})
        {
            available = component > max - available ? max : available + component;
        }
    }

    return MemoryStats {total, std::min(total, available)};
}

std::optional<MemoryStats> read_memory_stats(const std::string& path)
{
    std::ifstream input(path);
    if (!input)
    {
        return std::nullopt;
    }

    return parse_memory_stats(input);
}
