#include "console.h"
#include "map.h"
#include "util.h"
#include <iostream>
#include <optional> 

int main(char** argv, int argc) {
    gfx::Console console(120, 60, "data/font/kongtext.ttf");

    game::Map map(80, 50);
    map.generate();

    // quick & dirty player
    struct Player {
        uint32_t x = 5;
        uint32_t y = 5;
        gfx::Console::glyph_t glyph = {'@', sf::Color::Yellow, sf::Color::Black};

        bool can_move_to(const game::Map& map, const int32_t x, const int32_t y) const noexcept {
            sf::Vector2u size = map.get_size();
            if (x < 0 || y < 0 || x >= size.x || y >= size.y) {
                return false;
            }
            const auto& tile = map.get_tile(x, y);
            return (tile.type == game::Map::Tile::Type::Floor);
        }
    } player;

    while (true) {
        map.render_to(console, 8, 8);
        console.set_glyph(player.x + 8, player.y + 8, player.glyph);
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

                    switch (event.key.code) {
                        // HJKL vi-movement
                        case sf::Keyboard::H: {
                            if (player.can_move_to(map, player.x - 1, player.y)) {
                                player.x -= 1;
                            }
                        } break;
                        case sf::Keyboard::J: {
                            if (player.can_move_to(map, player.x, player.y + 1)) {
                                player.y += 1;
                            }
                        } break;
                        case sf::Keyboard::K: {
                            if (player.can_move_to(map, player.x, player.y - 1)) {
                                player.y -= 1;
                            }
                        } break;
                        case sf::Keyboard::L: {
                            if (player.can_move_to(map, player.x + 1, player.y)) {
                                player.x += 1;
                            }
                        } break;
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