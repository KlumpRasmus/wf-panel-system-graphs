#include "usage_graph.hpp"

#include "memory_stats.hpp"

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>

namespace
{
constexpr char PLUGIN_TITLE[] = "Memory";

static constexpr conf_table_t conf_table[] = {
    {CONF_TYPE_BOOL, "show_percentage", N_("Show usage as percentage"), nullptr},
    {CONF_TYPE_COLOUR, "foreground", N_("Foreground colour"), nullptr},
    {CONF_TYPE_COLOUR, "background", N_("Background colour"), nullptr},
    {CONF_TYPE_NONE, nullptr, nullptr, nullptr},
};

std::string format_size(std::uint64_t kib)
{
    std::ostringstream value;
    if (kib >= 1024ULL * 1024ULL)
    {
        value << std::fixed << std::setprecision(1)
              << static_cast<double>(kib) / (1024.0 * 1024.0) << " GiB";
    } else
    {
        value << std::fixed << std::setprecision(0)
              << static_cast<double>(kib) / 1024.0 << " MiB";
    }
    return value.str();
}

class MemoryWidget final : public UsageGraphWidget
{
  public:
    MemoryWidget()
        : UsageGraphWidget(
              "memory", "memory", "memory", "M",
              "Memory information unavailable")
    {}

  protected:
    std::optional<UsageSample> read_sample() override
    {
        const auto stats = read_memory_stats();
        if (!stats)
        {
            return std::nullopt;
        }

        const auto fraction = stats->used_fraction();
        const auto percent = static_cast<int>(std::lround(fraction * 100.0));
        std::ostringstream tooltip;
        tooltip << "Memory: " << format_size(stats->used_kib()) << " used of "
                << format_size(stats->total_kib) << " (" << percent << "%)";
        return UsageSample {fraction, tooltip.str()};
    }
};
} // namespace

extern "C"
{
WayfireWidget *create()
{
    return new MemoryWidget;
}

void destroy(WayfireWidget *widget)
{
    delete widget;
}

const conf_table_t *config_params()
{
    return conf_table;
}

const char *display_name()
{
    return PLUGIN_TITLE;
}

const char *package_name()
{
    return GETTEXT_PACKAGE;
}
}
