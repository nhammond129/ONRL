#include "console.h"
#include "map.h"
#include "util.h"
#include "colors.h"
#include "player.h"
#include <iostream>
#include <optional>


#define GW_OFF_X 32
#define GW_OFF_Y 0

void render_player(gfx::Console& console, const game::Player& player) noexcept {
    console.set_glyph(player.x + GW_OFF_X, player.y + GW_OFF_Y, {player.c, player.fg, console.get_glyph(player.x + GW_OFF_X, player.y + GW_OFF_Y).bg});
}

int main(char** argv, int argc) {
    // seed random
    srand(time(NULL));
    gfx::Console console(120, 60, "data/font/kongtext.ttf");

    game::Map map(88, 50);
    game::Player player;
    map.generate();

    // find a random passable tile
    std::optional<sf::Vector2u> player_pos;
    while (!player_pos.has_value()) {
        const auto x = rand() % map.get_size().x;
        const auto y = rand() % map.get_size().y;
        if (map.get_tile(x, y).passable) {
            player_pos = sf::Vector2u{x, y};
        }
    }
    player.x = player_pos.value().x;
    player.y = player_pos.value().y;

    // game loop
    while (true) {
        map.render_to(console, GW_OFF_X, GW_OFF_Y);
        render_player(console, player);
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
                        case sf::Keyboard::H: player.try_move_to(map, player.x - 1, player.y); break;
                        case sf::Keyboard::J: player.try_move_to(map, player.x, player.y + 1); break;
                        case sf::Keyboard::K: player.try_move_to(map, player.x, player.y - 1); break;
                        case sf::Keyboard::L: player.try_move_to(map, player.x + 1, player.y); break;
                    }
                } break;
                default:
                    // std::cout << "Unhandled event: " << util::sf::to_string(event.type) << std::endl;
                    break;
            }
        }
    }
    return 1;
}