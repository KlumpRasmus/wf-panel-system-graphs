#include "colour_config.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string>
#include <utility>

namespace
{
std::string trim(std::string value)
{
    const auto not_space = [](unsigned char character) {
        return !std::isspace(character);
    };
    value.erase(value.begin(), std::find_if(value.begin(), value.end(), not_space));
    value.erase(
        std::find_if(value.rbegin(), value.rend(), not_space).base(),
        value.end());
    return value;
}
} // namespace

const std::optional<std::string>& ColourConfig::foreground_for(
    const std::string& key) const
{
    if (key == "cpu")
    {
        return cpu;
    }
    if (key == "gpu")
    {
        return gpu;
    }
    return memory;
}

ColourConfig parse_colour_config(std::istream& input)
{
    ColourConfig config;
    std::string line;
    while (std::getline(input, line))
    {
        line = trim(std::move(line));
        if (line.empty() || line.front() == ';')
        {
            continue;
        }

        const auto separator = line.find('=');
        if (separator == std::string::npos)
        {
            continue;
        }

        const auto key = trim(line.substr(0, separator));
        const auto value = trim(line.substr(separator + 1));
        if (value.empty())
        {
            continue;
        }

        std::optional<std::string> *destination = nullptr;
        if (key == "cpu")
        {
            destination = &config.cpu;
        } else if (key == "gpu")
        {
            destination = &config.gpu;
        } else if (key == "memory")
        {
            destination = &config.memory;
        } else if (key == "background")
        {
            destination = &config.background;
        } else if (key == "text")
        {
            destination = &config.text;
        } else if (key == "shadow")
        {
            destination = &config.shadow;
        }

        if (destination)
        {
            *destination = value;
        }
    }
    return config;
}

ColourConfig read_colour_config(const std::string& path)
{
    std::ifstream input(path);
    if (!input)
    {
        return {};
    }
    return parse_colour_config(input);
}
