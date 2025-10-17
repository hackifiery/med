#include <iostream>
#include <unistd.h>

using namespace std;

char readKey() {
    char c;
    if (read(STDIN_FILENO, &c, 1) != 1) return 0;

    if (c == 27) { // Escape sequence
        char seq[6];
        // read up to 5 bytes after ESC
        int n = read(STDIN_FILENO, &seq, sizeof(seq));

        if (n >= 2 && seq[0] == '[') {
            if (n == 2) {
                switch (seq[1]) {
                    case 'A': return 'U'; // Up
                    case 'B': return 'D'; // Down
                    case 'C': return 'R'; // Right
                    case 'D': return 'L'; // Left
                    case 'H': return 'H'; // Home
                    case 'F': return 'E'; // End
                }
            }
            else if (n >= 5 && seq[0] == '1' && seq[1] == ';' && seq[2] == '5') {
                if (seq[3] == 'H') return 'h'; // Ctrl+Home
                if (seq[3] == 'F') return 'e'; // Ctrl+End
            }
        }

        return '?'; // unknown escape
    }

    return c;
}
