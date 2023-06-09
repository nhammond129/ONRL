#include "util.h"

namespace util {

void log(std::string message, std::source_location location) noexcept {
    std::cout << location.file_name() << ":" << location.line() << ":" << location.column() << " " << message << std::endl;
}

void halt_catch_fire(std::string message, std::source_location location) {
    log(message, location);
    throw std::runtime_error(message);
}

float distance(::sf::Vector2u a, ::sf::Vector2u b) {
    return std::sqrt(std::pow(a.x - b.x, 2) + std::pow(a.y - b.y, 2));
}

namespace sf {
    std::string to_string(::sf::Event::EventType etype)  {
        switch (etype) {
            case ::sf::Event::Closed: return "Closed";
            case ::sf::Event::Resized: return "Resized";
            case ::sf::Event::LostFocus: return "LostFocus";
            case ::sf::Event::GainedFocus: return "GainedFocus";
            case ::sf::Event::TextEntered: return "TextEntered";
            case ::sf::Event::KeyPressed: return "KeyPressed";
            case ::sf::Event::KeyReleased: return "KeyReleased";
            case ::sf::Event::MouseWheelMoved: return "MouseWheelMoved";
            case ::sf::Event::MouseWheelScrolled: return "MouseWheelScrolled";
            case ::sf::Event::MouseButtonPressed: return "MouseButtonPressed";
            case ::sf::Event::MouseButtonReleased: return "MouseButtonReleased";
            case ::sf::Event::MouseMoved: return "MouseMoved";
            case ::sf::Event::MouseEntered: return "MouseEntered";
            case ::sf::Event::MouseLeft: return "MouseLeft";
            case ::sf::Event::JoystickButtonPressed: return "JoystickButtonPressed";
            case ::sf::Event::JoystickButtonReleased: return "JoystickButtonReleased";
            case ::sf::Event::JoystickMoved: return "JoystickMoved";
            case ::sf::Event::JoystickConnected: return "JoystickConnected";
            case ::sf::Event::JoystickDisconnected: return "JoystickDisconnected";
            case ::sf::Event::TouchBegan: return "TouchBegan";
            case ::sf::Event::TouchMoved: return "TouchMoved";
            case ::sf::Event::TouchEnded: return "TouchEnded";
            case ::sf::Event::SensorChanged: return "SensorChanged";
            case ::sf::Event::Count: return "COUNT_SENTINEL";
            default: return "UNKNOWN";
        }
    }
}  // namespace sf

}  // namespace util