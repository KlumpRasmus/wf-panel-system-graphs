#include "usage_graph.hpp"

#include "cpu_sampler.hpp"

#include <cmath>
#include <optional>
#include <string>

namespace
{
constexpr char PLUGIN_TITLE[] = "CPU Custom";

static constexpr conf_table_t conf_table[] = {
    {CONF_TYPE_BOOL, "show_percentage", N_("Show usage as percentage"), nullptr},
    {CONF_TYPE_COLOUR, "foreground", N_("Foreground colour"), nullptr},
    {CONF_TYPE_COLOUR, "background", N_("Background colour"), nullptr},
    {CONF_TYPE_NONE, nullptr, nullptr, nullptr},
};

class CpuWidget final : public UsageGraphWidget
{
  public:
    CpuWidget()
        : UsageGraphWidget(
              "cpugraph", "cpugraph", "C", "CPU information unavailable")
    {}

  protected:
    std::optional<UsageSample> read_sample() override
    {
        const auto fraction = sampler.sample();
        if (!fraction)
        {
            return std::nullopt;
        }

        const auto percent = static_cast<int>(std::lround(*fraction * 100.0));
        return UsageSample {
            *fraction, "CPU usage: " + std::to_string(percent) + "%"};
    }

  private:
    CpuSampler sampler;
};
} // namespace

extern "C"
{
WayfireWidget *create()
{
    return new CpuWidget;
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
