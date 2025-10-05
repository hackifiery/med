#pragma once

void enableRawMode(struct termios orig_termios);
void disableRawMode(struct termios orig_termios);