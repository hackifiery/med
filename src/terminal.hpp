#include <termios.h>
#include <unistd.h>


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