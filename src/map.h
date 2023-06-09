#pragma once

#include <array>
#include "console.h"  // for Console::glyph_t
#include "colors.h"
#include <optional>

namespace game {

class Unit;

class Map {
    using glyph_t = gfx::Console::glyph_t;
public:
    struct Tile {
        glyph_t get_glyph() noexcept {
            if (!passable) bg = fg;
            if (mossification > 0.0) {
                return glyph_t{',' , Color::DGREEN, bg};
            }
            return glyph_t{c, fg, bg};
        }
        char c = '.';
        sf::Color fg = Color::LGRAY;
        sf::Color bg = Color::DGRAY;
        bool passable = false;
        float mossification = 0;
        game::Unit* unit_occupying = nullptr;
    };
    Map(const size_t width, const size_t height) : width(width), height(height) {}
    void generate();
    void render_to(gfx::Console& console, uint32_t x, uint32_t y) noexcept;
    sf::Vector2u get_size() const;
    const Tile& get_tile(const size_t x, const size_t y) const;
    Tile& get_tile_mut(const size_t x, const size_t y);
    std::optional<std::vector<sf::Vector2u>> find_path(const sf::Vector2u& start, const sf::Vector2u& end) const;
    sf::Vector2u get_random_passable() const;
    std::vector<sf::Vector2u> get_xys_within_radius(const sf::Vector2u& xy, const uint32_t radius) const;
private:
    const uint32_t width;
    const uint32_t height;
    std::vector<Tile> tiles = {};
};

}  // namespace game