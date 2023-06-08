#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>

namespace gfx {

/**
 * @brief A console is a window that displays text.
 * 
 * The console is a window that displays text. It is a fixed size, and has a fixed font.
 * Characters are rendered as 8x8 pixel blocks.
 * 
 * FUTURE: Could extend for subregions of the console with {x, y, width, height, stride}?
 *         TL_glyphidx = x + y * stride
 *         BR_glyphidx = (x + width) + (y + height) * stride
 */
class Console {
public:
    struct glyph_t {
        char c;
        sf::Color fg;
        sf::Color bg;
    };
    Console(uint32_t width, uint32_t height, std::string font_path, uint32_t font_size = 12);

    void render() noexcept;
    void set_glyph(uint32_t x, uint32_t y, glyph_t g);
    void set_region(uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::vector<glyph_t> glyphs);
    template <size_t N>
    void set_region(uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::array<glyph_t, N> glyphs);
    sf::Vector2u get_mouse_tile_xy() const;
    bool poll_event(sf::Event& event) noexcept;
private:
    uint32_t width;
    uint32_t height;
    uint32_t glyph_px_width;
    uint32_t glyph_px_height;
    uint32_t glyph_v_edge_pad;   // edge padding so that the glyph grid has uniform spacing on each edge :)
    uint32_t glyph_h_edge_pad;

    sf::RenderWindow window;
    sf::Font font;
    uint32_t font_size;

    std::vector<glyph_t> glyphs;
};

}  // namespace gfx