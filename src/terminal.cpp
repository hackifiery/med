#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <utility>
#include "terminal.hpp"

using namespace std;

void enableRawMode(struct termios orig_termios) {
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_iflag &= ~(IXON);  // <--- disable Ctrl-S / Ctrl-Q flow control
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void disableRawMode(struct termios orig_termios) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
pair<int, int> getWindowSize() {
    int rows, cols;
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        rows = 24;
        cols = 80;
    } else {
        rows = ws.ws_row;
        cols = ws.ws_col;
    }
    return make_pair(rows, cols);
}

void clearScreen(){
    cout << "\033[?1049h";
    cout.flush();
}

void restoreScreen(){
    cout << "\033[?1049l";
    cout.flush();
}