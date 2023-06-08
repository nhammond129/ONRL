#include "console.h"
#include "util.h"

namespace gfx {

Console::Console(uint32_t width, uint32_t height, std::string font_path, uint32_t font_size) : width(width), height(height), font_size(font_size) {
    if (!font.loadFromFile(font_path)) {
        util::halt_catch_fire("Failed to load font from " + font_path);
    }

    // get size of one char
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(font_size);
    text.setString("A");
    sf::FloatRect text_rect = text.getLocalBounds();

    // log text_rect
    util::log("Text rect is at " + std::to_string(text_rect.left) + "," + std::to_string(text_rect.top)
            + " with size " + std::to_string(text_rect.width) + "x" + std::to_string(text_rect.height));

    uint32_t padding = std::max(text_rect.left, text_rect.top);

    glyph_px_width  = text_rect.width  + padding*2;
    glyph_px_height = text_rect.height + padding*2;

    // hard to read but a quick n dirty way to match edge padding
    glyph_v_edge_pad = (text_rect.left - text_rect.top ) > 0 ? (text_rect.left - text_rect.top ) : 0;
    glyph_h_edge_pad = (text_rect.top  - text_rect.left) > 0 ? (text_rect.top  - text_rect.left) : 0;

    // log some window stuff
    util::log("Console created with width " + std::to_string(width) + " and height " + std::to_string(height));
    util::log("Glyph size is " + std::to_string(glyph_px_width) + "x" + std::to_string(glyph_px_height)
            + " with font size " + std::to_string(font_size));
    util::log("Window size will be " + std::to_string(width * glyph_px_width) + "x" + std::to_string(height * glyph_px_height));

    // create window based on that glyph_t's size
    // WARN:
    //  This makes the assumption that the font has a fixed width and height for all characters.
    //  This is not true for all fonts, and might cause some issues if the font does not have a fixed width and height.
    //  FUTURE:
    //   This could be fixed by taking the max width and height of all characters in the font, and using that as the size.
    // TODO: Window name as parameter.
    window.create(
        sf::VideoMode(
            width  * glyph_px_width  + 2*glyph_h_edge_pad,
            height * glyph_px_height + 2*glyph_v_edge_pad
        ),
        "Console"
    );

    // reserve space for all glyphs
    glyphs.reserve(width * height);

    // initialize all glyphs to be some default
    for (uint32_t i = 0; i < width * height; i++) {
        sf::Color fg = sf::Color{211, 211, 211};
        const char c = '.';
        glyphs.push_back(
            {fg, sf::Color{ 34,  34,  34}, c}
        );
    }
}

void Console::render() noexcept {
    window.clear();

    // simple render of all glyphs
    // NOTE: glyph_*_edge_pad gutters are always black
    // FUTURE: This could be fixed by padding out the size of the edge glyphs to cover the 'gutter'.

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(font_size);

    sf::RectangleShape bg_rect;
    bg_rect.setSize(sf::Vector2f(glyph_px_width, glyph_px_height));

    for (uint32_t i = 0; i < width * height; i++) {
        uint32_t glyph_x = glyph_h_edge_pad + (i%width) * glyph_px_width;
        uint32_t glyph_y = glyph_v_edge_pad + (i/width) * glyph_px_height;
        // draw background
        bg_rect.setPosition(glyph_x, glyph_y);
        bg_rect.setFillColor(glyphs[i].bg);
        window.draw(bg_rect);
        text.setFillColor(glyphs[i].fg);
        text.setString(glyphs[i].c);
        text.setPosition(glyph_x, glyph_y);
        window.draw(text);
    }

    window.display();
}

void Console::set_glyph(uint32_t x, uint32_t y, Console::glyph_t glyph) {
    if (x >= width || y >= height) {
        util::halt_catch_fire("Attempted to set glyph at " + std::to_string(x) + "," + std::to_string(y)
                + " but console size is " + std::to_string(width) + "x" + std::to_string(height));
    }
    glyphs[y*width + x] = glyph;
}

void Console::set_region(uint32_t x, uint32_t y, uint32_t w, uint32_t h, std::vector<Console::glyph_t> glyphs) {
    if (x >= width || y >= height) {
        util::halt_catch_fire("Attempted to set region at " + std::to_string(x) + "," + std::to_string(y)
                + " but console size is " + std::to_string(width) + "x" + std::to_string(height));
    }
    if (x + w >= width || y + h >= height) {
        util::halt_catch_fire("Attempted to set region at " + std::to_string(x) + "," + std::to_string(y)
                + " with size " + std::to_string(w) + "x" + std::to_string(h)
                + " but console size is " + std::to_string(width) + "x" + std::to_string(height));
    }
    if (glyphs.size() != w * h) {
        util::halt_catch_fire("Attempted to set region at " + std::to_string(x) + "," + std::to_string(y)
                + " with size " + std::to_string(w) + "x" + std::to_string(h) + "=" + std::to_string(w*h) + " glyphs"
                + " but glyphs vector size is " + std::to_string(glyphs.size()));
    }
    for (uint32_t i = 0; i < w*h; i++) {
        this->glyphs[(y+i/w)*width + (x+i%w)] = glyphs[i];
    }
}

sf::Vector2u Console::get_mouse_tile_xy() const {
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);
    uint32_t mouse_x = mouse_pos.x - glyph_h_edge_pad;
    uint32_t mouse_y = mouse_pos.y - glyph_v_edge_pad;
    uint32_t tile_x = mouse_x / glyph_px_width;
    uint32_t tile_y = mouse_y / glyph_px_height;
    return sf::Vector2u(tile_x, tile_y);
}

}  // namespace gfx