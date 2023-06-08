#include "map.h"
#include <cstdlib>
#include <cassert>

namespace game {

namespace {

// make a room.
void make_room(std::vector<Map::Tile>& tiles, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t stride) {
    for (uint32_t i = x + 1; i < x + w - 1; i++) {
        for (uint32_t j = y + 1; j < y + h - 1; j++) {
            tiles[j*stride + i].passable = true;
        }
    }
}

/**
 * @brief BSP tree map generation
 * @param tiles the tiles vec to operate on
 * @param split_factor the factor to split the rectangle by (from 0.5-x to 0.5+x)
 * we split the rectangle into random 'halves' recursively
 * until we reach a minimum size
 * then we fill the rectangles with walls,
 **/
bool BSP_recurse_region(
        std::vector<Map::Tile>& tiles,
        uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t stride,
        float split_factor = 0.2) {
    static const uint32_t MIN_WIDTH  = 4;
    static const uint32_t MIN_HEIGHT = 4;
    //if the rectangle is too small, stop recursing
    if (w < MIN_WIDTH || h < MIN_HEIGHT) {
        return false;
    }

    // if the rectangle is wider than it is tall, split it vertically
    if (w > h) {
        // calculate the split point
        uint32_t split = x + w * (split_factor + (rand() % 100) / 100.0 * (1 - split_factor * 2));

        // recurse on the left and right halves
        bool left  = BSP_recurse_region(tiles, x, y, split - x, h, stride);
        bool right = BSP_recurse_region(tiles, split, y, w - (split - x), h, stride);

        // if either half is too small, make a room.
        if (!left || !right) make_room(tiles, x, y, w, h, stride);

        if (left && right) {
            // make a corridor between the two at a random y pos
            uint32_t corridor_y = y + (rand() % (h - 2)) + 1;
            for (uint32_t i = x + 1; i < x + w-1; i++) {
                tiles[corridor_y*stride + i].passable = true;
            }
        }
    } else {
        // calculate the split point
        uint32_t split = y + h * (split_factor + (rand() % 100) / 100.0 * (1 - split_factor * 2));

        // recurse on the top and bottom halves
        bool top = BSP_recurse_region(tiles, x, y, w, split - y, stride);
        bool bot = BSP_recurse_region(tiles, x, split, w, h - (split - y), stride);

        // if either half is too small, make a room.
        if (!top || !bot) make_room(tiles, x, y, w, h, stride);

        if (top && bot) {
            // make a corridor between the two at a random horizontal position
            uint32_t corridor_x = x + (rand() % (w - 2)) + 1;
            for (uint32_t i = y + 1; i < y + h-1; i++) {
                tiles[i*stride + corridor_x].passable = true;
            }
        }
    }
    return true;
}

std::vector<uint8_t> CA_count_neighbors(const std::vector<Map::Tile>& tiles, uint32_t w, uint32_t h) {
    auto get_neighbors = [&](const std::vector<Map::Tile>& tiles, uint32_t x, uint32_t y) {
        if (x == 0 || y == 0 || x == w-1 || y == h-1) return uint8_t(-1);
        uint8_t neighbors = 0;
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if (i == 0 && j == 0) continue;
                if (tiles[(y+j)*w + (x+i)].passable == false) neighbors++;
            }
        }
        return neighbors;
    };
    std::vector<uint8_t> neighbors(w*h);
    for (uint32_t i = 0; i < w*h; i++) {
        uint32_t x = i%w;
        uint32_t y = i/w;
        neighbors[i] = get_neighbors(tiles, x, y);
    }
    return neighbors;
}

void CA_anneal_region_impl(std::vector<Map::Tile>& tiles, const uint32_t w, const uint32_t h) {
    std::vector<uint8_t> neighbors = CA_count_neighbors(tiles, w, h);
    for (uint32_t i = 0; i < w*h; i++) {
        uint32_t x = i%w;
        uint32_t y = i/w;
        if (neighbors[i] < 4) tiles[i].passable = true;
        else if (neighbors[i] > 4) tiles[i].passable = false;
    }
}

void CA_anneal_region(std::vector<Map::Tile>& tiles, const uint32_t w, const uint32_t h, const uint32_t passes) {
    for (uint32_t i = 0; i < passes; i++) {
        CA_anneal_region_impl(tiles, w, h);
    }
}

}  // namespace anonymous

void Map::generate() {
    tiles.resize(width*height);
    for (size_t i = 0; i < width*height; ++i) tiles[i].passable = false;
    
    // base structure
    BSP_recurse_region(tiles, 0, 0, width, height, width);

    // annealing
    CA_anneal_region(tiles, width, height, 4);
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