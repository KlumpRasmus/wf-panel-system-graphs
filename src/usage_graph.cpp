#include "usage_graph.hpp"

#include <gtk-utils.hpp>
#include <cairomm/context.h>
#include <glib.h>
#include <glibmm/main.h>

#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
void set_source_rgba(
    const Cairo::RefPtr<Cairo::Context>& context, const Gdk::RGBA& colour)
{
    context->set_source_rgba(
        colour.get_red(), colour.get_green(), colour.get_blue(),
        colour.get_alpha());
}
} // namespace

UsageGraphWidget::UsageGraphWidget(
    std::string plugin_id_value, std::string option_prefix,
    std::string label_prefix_value, std::string unavailable_text_value)
    : plugin_id(std::move(plugin_id_value)),
      label_prefix(std::move(label_prefix_value)),
      unavailable_text(std::move(unavailable_text_value)),
      show_percentage("panel/" + option_prefix + "_show_percentage"),
      foreground_colour("panel/" + option_prefix + "_foreground"),
      background_colour("panel/" + option_prefix + "_background")
{}

UsageGraphWidget::~UsageGraphWidget()
{
    timer.disconnect();
}

void UsageGraphWidget::init(Gtk::HBox *container)
{
    plugin = std::make_unique<Gtk::Button>();
    plugin->set_name(plugin_id);
    plugin->set_relief(Gtk::RELIEF_NONE);
    container->pack_start(*plugin, false, false);

    graph = Gtk::manage(new Gtk::DrawingArea());
    plugin->add(*graph);
    graph->signal_draw().connect(sigc::mem_fun(*this, &UsageGraphWidget::draw));
    plugin->signal_clicked().connect(
        sigc::mem_fun(*this, &UsageGraphWidget::open_lxtask));
    gesture = add_longpress_default(*plugin);

    read_settings();
    update_size();
    update();

    show_percentage.set_callback(
        sigc::mem_fun(*this, &UsageGraphWidget::settings_changed));
    foreground_colour.set_callback(
        sigc::mem_fun(*this, &UsageGraphWidget::settings_changed));
    background_colour.set_callback(
        sigc::mem_fun(*this, &UsageGraphWidget::settings_changed));
    icon_size.set_callback(
        sigc::mem_fun(*this, &UsageGraphWidget::settings_changed));

    timer = Glib::signal_timeout().connect(
        sigc::mem_fun(*this, &UsageGraphWidget::update), UPDATE_INTERVAL_MS);
    plugin->show_all();
}

void UsageGraphWidget::handle_config_reload()
{
    settings_changed();
}

void UsageGraphWidget::read_settings()
{
    if (!foreground.set(static_cast<std::string>(foreground_colour)))
    {
        foreground.set("#53c86b");
    }

    if (!background.set(static_cast<std::string>(background_colour)))
    {
        background.set("rgba(64,64,64,0.55)");
    }
}

void UsageGraphWidget::settings_changed()
{
    read_settings();
    update_size();
    if (graph)
    {
        graph->queue_draw();
    }
}

void UsageGraphWidget::update_size()
{
    if (!graph)
    {
        return;
    }

    const int size = std::max(16, static_cast<int>(icon_size));
    const int text_room = static_cast<bool>(show_percentage) ? 38 : 0;
    graph->set_size_request(size + text_room, size);
}

bool UsageGraphWidget::update()
{
    const auto sample = read_sample();
    if (!sample)
    {
        have_current = false;
        plugin->set_tooltip_text(unavailable_text);
        return true;
    }

    current_fraction = std::clamp(sample->fraction, 0.0, 1.0);
    have_current = true;
    history.push_back(current_fraction);
    while (history.size() > MAX_SAMPLES)
    {
        history.pop_front();
    }

    plugin->set_tooltip_text(sample->tooltip + "\nClick to open LXTask");
    graph->queue_draw();
    return true;
}

bool UsageGraphWidget::draw(const Cairo::RefPtr<Cairo::Context>& context)
{
    const auto allocation = graph->get_allocation();
    const double width = allocation.get_width();
    const double height = allocation.get_height();

    set_source_rgba(context, background);
    context->rectangle(0.0, 0.0, width, height);
    context->fill();

    if (!history.empty())
    {
        set_source_rgba(context, foreground);
        context->move_to(0.0, height);
        if (history.size() == 1)
        {
            context->line_to(0.0, height * (1.0 - history.front()));
            context->line_to(width, height * (1.0 - history.front()));
        } else
        {
            for (std::size_t index = 0; index < history.size(); ++index)
            {
                const double x = width * static_cast<double>(index) /
                    static_cast<double>(history.size() - 1);
                const double y = height * (1.0 - history[index]);
                context->line_to(x, y);
            }
        }
        context->line_to(width, height);
        context->close_path();
        context->fill();
    }

    if (static_cast<bool>(show_percentage) && have_current)
    {
        const auto percent = static_cast<int>(
            std::lround(current_fraction * 100.0));
        const std::string text = label_prefix + ":" +
            std::to_string(percent) + "%";

        context->select_font_face(
            "Sans", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
        context->set_font_size(std::max(9.0, height * 0.34));

        Cairo::TextExtents extents;
        context->get_text_extents(text, extents);
        const double x = (width - extents.width) / 2.0 - extents.x_bearing;
        const double y = (height - extents.height) / 2.0 - extents.y_bearing;

        Gdk::RGBA shadow_colour;
        shadow_colour.set("rgba(0,0,0,0.65)");
        set_source_rgba(context, shadow_colour);
        context->move_to(x + 1.0, y + 1.0);
        context->show_text(text);

        Gdk::RGBA text_colour;
        text_colour.set("white");
        set_source_rgba(context, text_colour);
        context->move_to(x, y);
        context->show_text(text);
    }

    return true;
}

void UsageGraphWidget::open_lxtask()
{
    GError *error = nullptr;
    if (!g_spawn_command_line_async("lxtask", &error))
    {
        const std::string message = error ? error->message : "unknown error";
        plugin->set_tooltip_text("Could not start LXTask: " + message);
        if (error)
        {
            g_error_free(error);
        }
    }
}
