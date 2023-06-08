#pragma once

#include <source_location>
#include <iostream>
#include <SFML/Window.hpp>

namespace util {

void log(std::string message, std::source_location location = std::source_location::current()) noexcept;
void halt_catch_fire(std::string message, std::source_location location = std::source_location::current());

namespace sf {
    std::string to_string(::sf::Event::EventType etype);
}  // namespace sf

}  // namespace util