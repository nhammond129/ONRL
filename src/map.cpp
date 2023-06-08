#include "map.h"
#include <cstdlib>

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

}  // namespace game