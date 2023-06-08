#pragma once

#include <source_location>
#include <iostream>

namespace util {

void log(std::string message, std::source_location location = std::source_location::current()) noexcept {
    std::cout << location.file_name() << ":" << location.line() << ":" << location.column() << " " << message << std::endl;
}

void halt_catch_fire(std::string message, std::source_location location = std::source_location::current()) {
    log(message, location);
    throw std::runtime_error(message);
}

}  // namespace util