#include "map.h"
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <limits>

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
    static const uint32_t MIN_WIDTH  = 5;
    static const uint32_t MIN_HEIGHT = 5;
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
            // make 3-wide corridor between the two at a random vertical position
            uint32_t corridor_y = y + (rand() % (h - 4)) + 2;
            for (uint32_t cy = corridor_y-1; cy <= corridor_y+1; cy++) {
                for (uint32_t i = x + 1; i < x + w-1; i++) {
                    tiles[cy*stride + i].passable = true;
                }
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
            // make 3-wide corridor between the two at a random horizontal position
            uint32_t corridor_x = x + (rand() % (w - 4)) + 2;
            for (uint32_t cx = corridor_x-1; cx <= corridor_x+1; cx++) {
                for (uint32_t i = y + 1; i < y + h-1; i++) {
                    tiles[i*stride + cx].passable = true;
                }
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
        else if (neighbors[i] >= 4) tiles[i].passable = false;
    }
}

void CA_anneal_region(std::vector<Map::Tile>& tiles, const uint32_t w, const uint32_t h, const uint32_t passes) {
    for (uint32_t i = 0; i < passes; i++) {
        CA_anneal_region_impl(tiles, w, h);
    }
}

void enmossify(std::vector<Map::Tile>& tiles, const uint32_t w, const uint32_t h) {
    const float mossfactor = 0.1;
    const uint32_t N_tiles = w*h;
    uint32_t moss_tiles = 0;

    while (moss_tiles < N_tiles * mossfactor) {
        uint32_t x = rand() % w;
        uint32_t y = rand() % h;
        Map::Tile& tile = tiles[y*w + x];
        if (tile.passable == true) {
            tile.mossification += 0.1;
            moss_tiles++;
        }
    }
}

}  // namespace anonymous

void Map::generate() {
    tiles.resize(width*height);
    for (size_t i = 0; i < width*height; ++i) tiles[i].passable = false;
    
    // base structure
    BSP_recurse_region(tiles, 0, 0, width, height, width, 0.4);

    // annealing
    CA_anneal_region(tiles, width, height, 4);

    // mossification
    enmossify(tiles, width, height);
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

Map::Tile& Map::get_tile_mut(const size_t x, const size_t y) {
    assert(
        x >= 0    && y >= 0     &&
        x < width && y < height &&
        "Map::get_tile: out of bounds"
    );
    return tiles[y*width + x];
}

// https://en.wikipedia.org/wiki/A*_search_algorithm
std::optional<std::vector<sf::Vector2u>> Map::find_path(const sf::Vector2u& start, const sf::Vector2u& end) const {
    std::vector<sf::Vector2u> path;
    std::vector<sf::Vector2u> open;
    std::vector<sf::Vector2u> closed;
    std::vector<sf::Vector2u> came_from(width*height);
    std::vector<uint32_t> g_score(width*height, std::numeric_limits<uint32_t>::max());
    std::vector<uint32_t> f_score(width*height, std::numeric_limits<uint32_t>::max());

    auto get_neighbors = [&](const sf::Vector2u& pos) {
        std::vector<sf::Vector2u> neighbors;
        if (pos.x > 0) neighbors.push_back({pos.x-1, pos.y});
        if (pos.y > 0) neighbors.push_back({pos.x, pos.y-1});
        if (pos.x < width-1) neighbors.push_back({pos.x+1, pos.y});
        if (pos.y < height-1) neighbors.push_back({pos.x, pos.y+1});
        return neighbors;
    };

    auto heuristic = [&](const sf::Vector2u& a, const sf::Vector2u& b) {
        return std::abs(int(a.x) - int(b.x)) + std::abs(int(a.y) - int(b.y));
    };

    auto reconstruct_path = [&](const sf::Vector2u& current) {
        sf::Vector2u pos = current;
        while (pos != start) {
            path.push_back(pos);
            pos = came_from[pos.y * width + pos.x];
        }
        std::reverse(path.begin(), path.end());
    };

    open.push_back(start);
    g_score[start.y * width + start.x] = 0;
    f_score[start.y * width + start.x] = heuristic(start, end);

    while (open.size()) {
        // find node with lowest f_score
        uint32_t lowest_f_score = std::numeric_limits<uint32_t>::max();
        sf::Vector2u current;
        for (const auto& node : open) {
            if (f_score[node.y * width + node.x] < lowest_f_score) {
                lowest_f_score = f_score[node.y * width + node.x];
                current = node;
            }
        }

        if (current == end) {
            reconstruct_path(current);
            return path;
        }

        open.erase(std::remove(open.begin(), open.end(), current), open.end());
        closed.push_back(current);

        for (const auto& neighbor : get_neighbors(current)) {
            // skip if impassable
            if (!get_tile(neighbor.x, neighbor.y).passable) continue;
            // skip if already visited
            if (std::find(closed.begin(), closed.end(), neighbor) != closed.end()) continue;

            // calculate tentative g_score
            uint32_t tentative_g_score = g_score[current.y * width + current.x] + 1;
            if (std::find(open.begin(), open.end(), neighbor) == open.end()) {
                open.push_back(neighbor);
            } else if (tentative_g_score >= g_score[neighbor.y * width + neighbor.x]) {
                continue;
            }

            came_from[neighbor.y * width + neighbor.x] = current;
              g_score[neighbor.y * width + neighbor.x] = tentative_g_score;
              f_score[neighbor.y * width + neighbor.x] = tentative_g_score + heuristic(neighbor, end);
        }
    }
    return std::nullopt;
}

sf::Vector2u Map::get_random_passable() const {
    while (true) {
        uint32_t x = rand() % width;
        uint32_t y = rand() % height;
        if (get_tile(x, y).passable) return {x, y};
    }
}

std::vector<sf::Vector2u> Map::get_xys_within_radius(const sf::Vector2u& pos, const uint32_t radius) const {
    std::vector<sf::Vector2u> xys;
    for (uint32_t y = 0; y < height; ++y) {
        for (uint32_t x = 0; x < width; ++x) {
            if (std::abs(int(x) - int(pos.x)) + std::abs(int(y) - int(pos.y)) <= int(radius)) {
                xys.push_back({x, y});
            }
        }
    }
    return xys;
}

}  // namespace game
