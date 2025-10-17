#include <iostream>
#include <unistd.h>
#include <termios.h>

using namespace std;
struct termios orig_termios;

char readKey() {
    char c;
    // Check for read error (or EOF)
    if (read(STDIN_FILENO, &c, 1) != 1) return 0;

    if (c == 27) { // Escape sequence
        char seq[6];
        // Read the rest of the sequence (up to 5 bytes)
        int n = read(STDIN_FILENO, &seq, sizeof(seq));


        // Note: n is the number of bytes read AFTER the initial ESC
        
        // Ensure we read at least a starting bracket
        if (n == -1 || n == 0) return 27; // Error or just ESC key pressed

        if (seq[0] == '[') {
            // Case 1: Simple 2-byte sequence (e.g., Arrows, simple Home/End)
            if (n == 2) {
                switch (seq[1]) {
                    case 'A': return 'U'; // Up Arrow
                    case 'B': return 'D'; // Down Arrow
                    case 'C': return 'R'; // Right Arrow
                    case 'D': return 'L'; // Left Arrow
                    // Note: Home/End are often sent as 3-byte sequences (ESC[H and ESC[F)
                    // but some terminals/configurations send them as simple 2-byte sequences
                    // We'll keep these for simple Home/End, but see Case 2 for full Home/End.
                    case 'H': return 'H'; // Simple Home
                    case 'F': return 'E'; // Simple End
                }
            } 
            // Case 2: Full Home/End (often 4 bytes: ESC[1~ and ESC[4~)
            else if (n == 3 && seq[1] == '1' && seq[2] == '~') {
                return 'H'; // Home (ESC[1~)
            }
            else if (n == 3 && seq[1] == '4' && seq[2] == '~') {
                return 'E'; // End (ESC[4~)
            }
            // Case 3: Control+Home/End (5-byte sequence after ESC: [1;5H and [1;5F)
            else if (n == 5 && seq[1] == '1' && seq[2] == ';' && seq[3] == '5') {
                if (seq[4] == 'H') return 'h'; // Ctrl+Home
                else if (seq[4] == 'F') return 'e'; // Ctrl+End
            }
            /* DEBUG
                for (char i : seq) {
                cout << i << " ";
                cout.flush();
            } */
        }
        
        return '?'; // unknown escape
    }

    return c;
}