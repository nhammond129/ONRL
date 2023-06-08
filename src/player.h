#pragma once
#include "map.h"
#include "colors.h"

namespace game {

struct Player {
    uint32_t x = 0;
    uint32_t y = 0;
    char c = '@';
    sf::Color fg = Color::YELLOW;

    bool can_move_to(const game::Map& map, const int32_t x, const int32_t y) const noexcept;
    bool try_move_to(const game::Map& map, const int32_t x, const int32_t y) noexcept;
};

}  // namespace game