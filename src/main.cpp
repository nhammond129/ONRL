#include "console.h"
#include "map.h"
#include "util.h"
#include "colors.h"
#include "unit.h"
#include <iostream>
#include <optional>


#define GW_OFF_X 16
#define GW_OFF_Y 0

void render_unit(gfx::Console& console, const game::Unit& unit) noexcept {
    console.set_glyph(unit.x + GW_OFF_X, unit.y + GW_OFF_Y, {unit.c, unit.fg, console.get_glyph(unit.x + GW_OFF_X, unit.y + GW_OFF_Y).bg});
}

sf::Vector2u get_random_unoccupied_tile(const game::Map& map, const game::Unit) {
    while (true) {
        const auto x = rand() % map.get_size().x;
        const auto y = rand() % map.get_size().y;
        if (map.get_tile(x, y).passable) {
            return sf::Vector2u{x, y};
        }
    }
}

enum class PLAYER_ACTION {
    MOVE_UP,
    MOVE_DOWN,
    MOVE_LEFT,
    MOVE_RIGHT,
    WAIT,
    CANCEL,
    NONE
};

PLAYER_ACTION get_player_action(const sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::K:       return PLAYER_ACTION::MOVE_UP;
        case sf::Keyboard::J:       return PLAYER_ACTION::MOVE_DOWN;
        case sf::Keyboard::H:       return PLAYER_ACTION::MOVE_LEFT;
        case sf::Keyboard::L:       return PLAYER_ACTION::MOVE_RIGHT;
        case sf::Keyboard::Space:   return PLAYER_ACTION::WAIT;
        case sf::Keyboard::Escape:  return PLAYER_ACTION::CANCEL;
        default: return PLAYER_ACTION::NONE;
    }
}

int main(char** argv, int argc) {
    srand(time(NULL));

    gfx::Console console(120, 60, "data/font/kongtext.ttf");

    game::Map map(104, 50);
    map.generate();

    game::Unit player('@', Color::YELLOW, false);
    // make 12 goblins
    game::Unit goblins[12];
    for (uint32_t i = 0; i < 12; ++i) {
        goblins[i] = {(i==0)?'G':'g', Color::COPPER, true};
        goblins[i].set_xy(map, map.get_random_passable());
    }

    player.set_xy(map, map.get_random_passable());

    // game loop
    while (true) {
        map.render_to(console, GW_OFF_X, GW_OFF_Y);
        render_unit(console, player);
        for (auto& goblin : goblins) {
            render_unit(console, goblin);
        }

        // find cursor xy
        const auto cursor_xy = console.get_mouse_tile_xy();
        // check if cursor is on the map
        if (cursor_xy.x >= GW_OFF_X
         && cursor_xy.x <  GW_OFF_X + map.get_size().x
         && cursor_xy.y >= GW_OFF_Y
         && cursor_xy.y <  GW_OFF_Y + map.get_size().y) {
            console.set_glyph(cursor_xy.x, cursor_xy.y, {console.get_glyph(cursor_xy.x, cursor_xy.y).c, Color::BLACK, Color::WHITE});
        }
        
        console.render();

        sf::Event event;
        PLAYER_ACTION action = PLAYER_ACTION::NONE;
        while (console.poll_event(event)) {
            switch (event.type) {
                case sf::Event::Closed: return 0;
                case sf::Event::KeyPressed:
                    if (event.key.control && event.key.code == sf::Keyboard::C) {
                        return 0;
                    }
                    action = get_player_action(event);
                default:
                    // std::cout << "Unhandled event: " << util::sf::to_string(event.type) << std::endl;
                    break;
            }
        }
        switch (action) {
            case PLAYER_ACTION::MOVE_UP:
                player.try_move_to(map, player.x, player.y - 1);
                game::Unit::accumulate_ai_time(player.speed);
                break;
            case PLAYER_ACTION::MOVE_DOWN:
                player.try_move_to(map, player.x, player.y + 1);
                game::Unit::accumulate_ai_time(player.speed);
                break;
            case PLAYER_ACTION::MOVE_LEFT:
                player.try_move_to(map, player.x - 1, player.y);
                game::Unit::accumulate_ai_time(player.speed);
                break;
            case PLAYER_ACTION::MOVE_RIGHT:
                player.try_move_to(map, player.x + 1, player.y);
                game::Unit::accumulate_ai_time(player.speed);
                break;
            case PLAYER_ACTION::WAIT:
                game::Unit::accumulate_ai_time(player.speed);
                break;
        }
        // iterate all units
        for (auto& unit : game::Unit::all_units) {
            while (unit->can_act()) {
                if (!unit->ai_controlled) continue;
                //** AI **//

                // find moss nearby
                std::optional<sf::Vector2u> moss = std::nullopt;
                // get all tiles in radius 4
                auto positions = map.get_xys_within_radius({unit->x, unit->y}, 4);
                // sort by distance
                std::sort(positions.begin(), positions.end(), [&](const auto& a, const auto& b) {
                    return util::distance({a.x, a.y}, {unit->x, unit->y}) < util::distance({b.x, b.y}, {unit->x, unit->y});
                });
                if (positions.size() > 0) {
                    for (const auto& pos : positions) {
                        if (map.get_tile(pos.x, pos.y).mossification > 0) {
                            moss = pos;
                            break;
                        }
                    }
                }
                // if moss found, move towards it
                if (moss) {
                    // draw line to moss
                    sf::Vertex line[] = {
                        sf::Vertex(sf::Vector2f(unit->x, unit->y), sf::Color::White),
                        sf::Vertex(sf::Vector2f(moss->x, moss->y), sf::Color::Green)
                    };
                    console.get_window().draw(line, 2, sf::Lines);
                    if (map.get_tile(unit->x, unit->y).mossification > 0) {
                        // if on moss, eat it. Why? idk. Moss.
                        map.get_tile_mut(moss->x, moss->y).mossification = 0;
                    }
                    const auto pathopt = map.find_path({unit->x, unit->y}, *moss);
                    if (pathopt && pathopt->size() > 0) {
                        const auto& path = *pathopt;
                        unit->try_move_to(map, path[0].x, path[0].y);
                    }
                } else {
                    // otherwise, move randomly
                    uint32_t x = unit->x;
                    uint32_t y = unit->y;
                    while (x == unit->x && y == unit->y) {
                        x += rand() % 3 - 1;
                        y += rand() % 3 - 1;
                    }
                    unit->try_move_to(map, x, y);
                }
                unit->time_accumulated -= unit->speed;
            }
        }


        console.window_display();
    }
    return 1;
}