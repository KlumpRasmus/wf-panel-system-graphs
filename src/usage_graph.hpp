#pragma once

#include <widget.hpp>

#include <gdkmm/rgba.h>
#include <gtkmm/button.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/gesturelongpress.h>

#include <cstddef>
#include <deque>
#include <memory>
#include <optional>
#include <string>

struct UsageSample
{
    double fraction = 0.0;
    std::string tooltip;
};

class UsageGraphWidget : public WayfireWidget
{
  public:
    UsageGraphWidget(
        std::string plugin_id, std::string option_prefix,
        std::string colour_key, std::string label_prefix,
        std::string unavailable_text);
    ~UsageGraphWidget() override;

    void init(Gtk::HBox *container) override;
    void handle_config_reload() override;

  protected:
    virtual std::optional<UsageSample> read_sample() = 0;

  private:
    bool update();
    bool draw(const Cairo::RefPtr<Cairo::Context>& context);
    void read_settings();
    void settings_changed();
    void update_size();
    void open_lxtask();

    std::string plugin_id;
    std::string colour_key;
    std::string label_prefix;
    std::string unavailable_text;

    std::unique_ptr<Gtk::Button> plugin;
    Gtk::DrawingArea *graph = nullptr;
    Glib::RefPtr<Gtk::GestureLongPress> gesture;
    sigc::connection timer;

    WfOption<bool> show_percentage;
    WfOption<std::string> foreground_colour;
    WfOption<std::string> background_colour;
    WfOption<int> icon_size {"panel/icon_size"};

    Gdk::RGBA foreground;
    Gdk::RGBA background;
    Gdk::RGBA text_colour;
    Gdk::RGBA shadow_colour;
    std::deque<double> history;
    double current_fraction = 0.0;
    bool have_current = false;

    static constexpr unsigned int UPDATE_INTERVAL_MS = 1500;
    static constexpr std::size_t MAX_SAMPLES = 120;
};
