#pragma once

#include "map.h"
#include "colors.h"

namespace game {

class Unit {
public:
    static std::vector<Unit*> all_units;  // NOTE: this is not safe. Use-after-free is possible!
    uint32_t x = 0;
    uint32_t y = 0;
    uint32_t speed = 10;
    uint32_t time_accumulated = 0;

    char c;
    sf::Color fg;

    bool ai_controlled = true;

    Unit(const char c, const sf::Color fg, bool ai = true) : c(c), fg(fg), ai_controlled(ai) {
        all_units.push_back(this);
    }
    Unit(): Unit('u', Color::MAGENTA) {}
    Unit operator=(const Unit& other) {
        this->x = other.x;
        this->y = other.y;
        this->speed = other.speed;
        this->time_accumulated = other.time_accumulated;
        this->c = other.c;
        this->fg = other.fg;
        this->ai_controlled = other.ai_controlled;
        return *this;
    }

    virtual ~Unit() {
        all_units.erase(std::remove(all_units.begin(), all_units.end(), this), all_units.end());
    }

    bool can_move_to(const game::Map& map, const uint32_t x, const uint32_t y) const;
    bool try_move_to(game::Map& map, const uint32_t x, const uint32_t y);
    bool can_act() const;
    void set_xy(game::Map& map, const sf::Vector2u& xy);

    static void accumulate_ai_time(const uint32_t timedelta) {
        for (auto& unit : all_units) {
            if (unit->ai_controlled) unit->time_accumulated += timedelta;
        }
    }
};

}  // namespace game