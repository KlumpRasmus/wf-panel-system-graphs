#include "cpu_sampler.hpp"
#include "gpu_sampler.hpp"

#include <cassert>
#include <cmath>
#include <sstream>

int main()
{
    {
        std::istringstream input(
            "cpu  10 20 30 40 5 2 2 1 0 0\n"
            "cpu0 1 2 3 4 0 0 0 0 0 0\n");
        const auto counters = parse_cpu_counters(input);
        assert(counters);
        assert(counters->total == 110);
        assert(counters->idle == 45);
    }

    {
        CpuSampler sampler;
        assert(sampler.sample_counters({100, 40}) == 0.0);
        const auto usage = sampler.sample_counters({200, 60});
        assert(usage);
        assert(std::abs(*usage - 0.8) < 0.000001);
    }

    {
        std::istringstream input(
            "bin 1000 2 200\n"
            "render 1000 3 400\n"
            "tfu 1000 0 0\n");
        const auto counters = parse_gpu_stats(input);
        assert(counters);
        assert(counters->timestamp == 1000);
        assert(counters->runtime[0] == 200);
        assert(counters->runtime[1] == 400);
    }

    {
        std::istringstream input(
            "timestamp;1000;\n"
            "v3d_bin;2;200;0;\n"
            "v3d_ren;3;400;0;\n");
        const auto counters = parse_legacy_gpu_usage(input);
        assert(counters);
        assert(counters->timestamp == 1000);
        assert(counters->runtime[0] == 200);
        assert(counters->runtime[1] == 400);
    }

    {
        GpuSampler sampler;
        GpuCounters first;
        first.timestamp = 1000;
        first.runtime = {200, 400, 0, 0, 0};
        first.present = {true, true, true, true, true};
        assert(sampler.sample_counters(first) == 0.0);

        GpuCounters second = first;
        second.timestamp = 1100;
        second.runtime = {220, 450, 0, 0, 0};
        const auto usage = sampler.sample_counters(second);
        assert(usage);
        assert(std::abs(*usage - 0.5) < 0.000001);
    }

    return 0;
}
