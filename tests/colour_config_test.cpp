#include "colour_config.hpp"

#include <cassert>
#include <sstream>

int main()
{
    std::istringstream input(R"(
; Values may contain # and commas.
cpu = #112233
gpu=rgba(1,2,3,0.5)
memory = green
background = #202020
text=white
shadow=rgba(0,0,0,0.65)
unknown=ignored
empty=
)");

    const auto config = parse_colour_config(input);
    assert(config.cpu == "#112233");
    assert(config.gpu == "rgba(1,2,3,0.5)");
    assert(config.memory == "green");
    assert(config.background == "#202020");
    assert(config.text == "white");
    assert(config.shadow == "rgba(0,0,0,0.65)");
    assert(config.foreground_for("cpu") == config.cpu);
    assert(config.foreground_for("gpu") == config.gpu);
    assert(config.foreground_for("memory") == config.memory);
}
