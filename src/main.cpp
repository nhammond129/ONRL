#include "console.h"

int main(char** argv, int argc) {
    gfx::Console console(3*5*5, 40, "data/font/kongtext.ttf");

    while (true) {
        console.render();
    }

    return 0;
}