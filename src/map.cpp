#include "map.h"
#include <cstdlib>
#include <cassert>

namespace game {

void Map::generate() {
    tiles.resize(width*height);
    srand(0);
    for (size_t i = 0; i < width*height; ++i) {
        if (rand() % 2 == 0) {
            tiles[i].type = Tile::Type::Wall;
        } else {
            tiles[i].type = Tile::Type::Floor;
        }
    }
}

void Map::render_to(gfx::Console& console, uint32_t x, uint32_t y) noexcept {
    for (size_t i = 0; i < width*height; ++i) {
        console.set_glyph(x + i%width, y + i/width, tiles[i].get_glyph());
    }
}

sf::Vector2u Map::get_size() const {
    return {width, height};
}

const Map::Tile& Map::get_tile(const size_t x, const size_t y) const {
    assert(
        x >= 0    && y >= 0     &&
        x < width && y < height &&
        "Map::get_tile: out of bounds"
    );
    return tiles[y*width + x];
}

}  // namespace game