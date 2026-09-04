#include "memory_stats.hpp"

#include <cassert>
#include <cmath>
#include <sstream>

int main()
{
    {
        std::istringstream input(
            "MemTotal:       8000000 kB\n"
            "MemFree:        1000000 kB\n"
            "MemAvailable:   6000000 kB\n"
            "Buffers:         200000 kB\n"
            "Cached:         1500000 kB\n");
        const auto stats = parse_memory_stats(input);
        assert(stats);
        assert(stats->total_kib == 8000000);
        assert(stats->available_kib == 6000000);
        assert(stats->used_kib() == 2000000);
        assert(std::abs(stats->used_fraction() - 0.25) < 0.000001);
    }

    {
        std::istringstream input(
            "MemTotal:       4000000 kB\n"
            "MemFree:         500000 kB\n"
            "Buffers:         100000 kB\n"
            "Cached:         1000000 kB\n"
            "SReclaimable:    200000 kB\n"
            "Shmem:           100000 kB\n");
        const auto stats = parse_memory_stats(input);
        assert(stats);
        assert(stats->available_kib == 1700000);
        assert(stats->used_kib() == 2300000);
    }

    {
        std::istringstream input("MemFree: 1000 kB\n");
        assert(!parse_memory_stats(input));
    }

    assert(read_memory_stats());
    return 0;
}
