#pragma once

#include <cstdint>
#include <istream>
#include <optional>
#include <string>

struct MemoryStats
{
    std::uint64_t total_kib = 0;
    std::uint64_t available_kib = 0;

    std::uint64_t used_kib() const;
    double used_fraction() const;
};

std::optional<MemoryStats> parse_memory_stats(std::istream& input);
std::optional<MemoryStats> read_memory_stats(
    const std::string& path = "/proc/meminfo");
