#include "player.h"

namespace game {

bool Player::can_move_to(const game::Map& map, const int32_t x, const int32_t y) const noexcept {
    if (x < 0 || y < 0 || x >= map.get_size().x || y >= map.get_size().y) return false;
    return map.get_tile(x, y).passable;
}

bool Player::try_move_to(const game::Map& map, const int32_t x, const int32_t y) noexcept {
    if (can_move_to(map, x, y)) {
        this->x = x;
        this->y = y;
        return true;
    }
    return false;
}

}  // namespace game