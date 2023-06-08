#pragma once

#include <array>
#include "console.h"  // for Console::glyph_t

namespace game {

class Map {
    using glyph_t = gfx::Console::glyph_t;
public:
    struct Tile {
        Tile() {
            type = Type::Floor;
        }
        union {
            struct Wall { } wall;
            struct Floor { } floor;
        };
        glyph_t get_glyph() const noexcept {
            switch (type) {
                case Type::Wall: return {'#', sf::Color::White, sf::Color::Black};
                case Type::Floor: return {'.', sf::Color::White, sf::Color::Black};
            }
        }
        enum class Type {
            Wall,
            Floor,
        } type;
    };
    Map(const size_t width, const size_t height) : width(width), height(height) {}
    void generate();
    void render_to(gfx::Console& console, uint32_t x, uint32_t y) noexcept;
private:
    const size_t width;
    const size_t height;
    std::vector<Tile> tiles = {};
};

}  // namespace game