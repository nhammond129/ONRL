#include "console.h"
#include "map.h"
#include "util.h"
#include <iostream>
#include <optional>

int main(char** argv, int argc) {
    gfx::Console console(120, 60, "data/font/kongtext.ttf");

    game::Map map(80, 50);
    map.generate();

    while (true) {
        map.render_to(console, 8, 8);
        console.render();
        sf::Event event;
        while (console.poll_event(event)) {
            switch (event.type) {
                case sf::Event::Closed: return 0;
                case sf::Event::KeyPressed: {
                    if (event.key.code == sf::Keyboard::Escape) {
                        return 0;
                    } else if (event.key.control && event.key.code == sf::Keyboard::C) {
                        return 0;
                    }
                } break;
                case sf::Event::MouseEntered:   break;
                case sf::Event::MouseMoved:     break;
                case sf::Event::MouseLeft:      break;
                case sf::Event::LostFocus:      break;
                default:
                    std::cout << "Unhandled event: " << util::sf::to_string(event.type) << std::endl;
                    break;
            }
        }
    }
    return 1;
}