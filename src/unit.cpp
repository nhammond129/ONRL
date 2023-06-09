#include "unit.h"

namespace game {

bool Unit::can_move_to(const game::Map& map, const uint32_t x, const uint32_t y) const {
    if (x < 0 || y < 0 || x >= map.get_size().x || y >= map.get_size().y) return false;
    return (\
         map.get_tile(x, y).passable && \
        (map.get_tile(x, y).unit_occupying == nullptr) \
    );
}

bool Unit::try_move_to(game::Map& map, const uint32_t x, const uint32_t y) {
    map.get_tile_mut(this->x, this->y).unit_occupying = nullptr;
    if (can_move_to(map, x, y)) {
        this->x = x;
        this->y = y;
        map.get_tile_mut(x, y).unit_occupying = this;
        return true;
    }
    return false;
}

bool Unit::can_act() const {
    return time_accumulated >= speed;
}

void Unit::set_xy(game::Map& map, const sf::Vector2u& xy) {
    try_move_to(map, xy.x, xy.y);
}

std::vector<Unit*> Unit::all_units = {};

}  // namespace game