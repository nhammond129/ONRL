#include "console.h"

int main(char** argv, int argc) {
    gfx::Console console(120, 60, "data/font/kongtext.ttf");

    while (true) {
        console.render();
    }

    return 0;
}