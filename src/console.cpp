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
    util::log("Text rect is at " + std::to_string(text_rect.left) + "x" + std::to_string(text_rect.top)
            + " with width " + std::to_string(text_rect.width) + " and height " + std::to_string(text_rect.height));

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
    util::log("Window size is " + std::to_string(width * glyph_px_width) + "x" + std::to_string(height * glyph_px_height));

    // create window based on that glyph's size
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
        sf::Color fg = sf::Color::White;
        char c;
        const uint8_t uwu = i%3;
        switch (uwu) {
            /* 'uwu' */
            case 0:
            case 2:  c = 'U'; break;
            case 1:  c = 'w'; break;
        }

        const uint8_t colors = (i/3)%5;
        switch (colors) {
            /* UPWPU */
            case 0:
            case 4:         fg = sf::Color{ 91, 206, 250}; break;  // light blue
            case 1:
            case 3:         fg = sf::Color{245, 169, 184}; break;  // pink
            case 2:         fg = sf::Color{255, 255, 255}; break;  // white
        }
        glyphs.push_back({fg, sf::Color::Black, c});
    }
}

void Console::render() noexcept {
    // clear window
    window.clear();

    // simple render of all glyphs
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(font_size);
    for (uint32_t i = 0; i < width * height; i++) {
        text.setFillColor(glyphs[i].fg);
        text.setOutlineColor(glyphs[i].bg);
        text.setString(glyphs[i].c);
        text.setPosition(
            glyph_h_edge_pad + (i%width) * glyph_px_width,
            glyph_v_edge_pad + (i/width) * glyph_px_height
        );
        window.draw(text);
    }

    // display window
    window.display();
}

}  // namespace gfx