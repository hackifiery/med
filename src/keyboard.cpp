/*
* Copyright (c) 2025 hackifiery. All rights reserved.
* All code licensed under the MIT License.
*/

#include <unistd.h>
#include <termios.h>
#include <utility>

using namespace std;
struct termios orig_termios;

pair<bool, char> readKey() {
    char c;
    // Check for read error (or EOF)
    if (read(STDIN_FILENO, &c, 1) != 1) return make_pair(true, 0);

    if (c == 27) { // Escape sequence
        char seq[6];
        // Read the rest of the sequence (up to 5 bytes)
        int n = read(STDIN_FILENO, &seq, sizeof(seq));


        // Note: n is the number of bytes read AFTER the initial ESC
        
        // Ensure we read at least a starting bracket
        if (n == -1 || n == 0) return make_pair(true, 27); // Error or just ESC key pressed

        if (seq[0] == '[') {
            // 2-byte seq
            if (n == 2) {
                switch (seq[1]) {
                    case 'A': return make_pair(true, 'U'); // Up Arrow
                    case 'B': return make_pair(true, 'D'); // Down Arrow
                    case 'C': return make_pair(true, 'R'); // Right Arrow
                    case 'D': return make_pair(true, 'L'); // Left Arrow
                    // Note: Home/End are often sent as 3-byte sequences (ESC[H and ESC[F)
                    // but some terminals/configurations send them as simple 2-byte sequences
                    case 'H': return make_pair(true, 'H'); // Simple Home
                    case 'F': return make_pair(true,'E'); // Simple End
                }
            } 
            // 4-byte seq
            else if (n == 3 && seq[1] == '1' && seq[2] == '~') {
                return make_pair(true, 'H'); // Home (ESC[1~)
            }
            else if (n == 3 && seq[1] == '4' && seq[2] == '~') {
                return make_pair(true, 'E'); // End (ESC[4~)
            }
            // 5-byte seq
            else if (n == 5 && seq[1] == '1' && seq[2] == ';' && seq[3] == '5') {
                if (seq[4] == 'H') return make_pair(true, 'h'); // Ctrl+Home
                else if (seq[4] == 'F') return make_pair(true, 'e'); // Ctrl+End
            }
            /* DEBUG
                for (char i : seq) {
                cout << i << " ";
                cout.flush();
            } */
        }
        
        return make_pair(true, '?'); // unknown escape
    }

    return make_pair(false, c);
}