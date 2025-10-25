#pragma once

#include <utility>

using namespace std;

void enableRawMode(struct termios orig_termios);
void disableRawMode(struct termios orig_termios);
pair<int, int> getWindowSize();
void clearScreen();
void restoreScreen();
