#pragma once

#include <istream>
#include <optional>
#include <string>

struct ColourConfig
{
    std::optional<std::string> cpu;
    std::optional<std::string> gpu;
    std::optional<std::string> memory;
    std::optional<std::string> background;
    std::optional<std::string> text;
    std::optional<std::string> shadow;

    const std::optional<std::string>& foreground_for(
        const std::string& key) const;
};

ColourConfig parse_colour_config(std::istream& input);
ColourConfig read_colour_config(const std::string& path);
