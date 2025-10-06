#include <iostream>
#include <unistd.h>

#include "keyboard.hpp"

using namespace std;

char readKey() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return 0;

    if (c == 27) { // Escape sequence
        char seq[2];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return 27;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return 27;

        if (seq[0] == '[') {
            switch (seq[1]) {
                case 'A': return 'U'; // Up
                case 'B': return 'D'; // Down
                case 'C': return 'R'; // Right
                case 'D': return 'L'; // Left
                default: return '?';
            }
        }
    }
    return c;
}