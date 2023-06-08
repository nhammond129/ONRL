#pragma once

#include <array>
#include "console.h"  // for Console::glyph_t
#include "colors.h"

namespace game {

class Map {
    using glyph_t = gfx::Console::glyph_t;
public:
    struct Tile {
        glyph_t get_glyph() noexcept {
            if (!passable) bg = fg;
            return glyph_t{c, fg, bg};
        }
        char c = '.';
        sf::Color fg = Color::LGRAY;
        sf::Color bg = Color::DGRAY;
        bool passable = false;
    };
    Map(const size_t width, const size_t height) : width(width), height(height) {}
    void generate();
    void render_to(gfx::Console& console, uint32_t x, uint32_t y) noexcept;
    sf::Vector2u get_size() const;
    const Tile& get_tile(const size_t x, const size_t y) const;
private:
    const uint32_t width;
    const uint32_t height;
    std::vector<Tile> tiles = {};
};

}  // namespace game