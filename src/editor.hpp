/*
* Copyright (c) 2025 hackifiery. All rights reserved.
* All code licensed under the MIT License.
*/

#pragma once

#include <termios.h>
#include <utility>
#include <vector>
#include <string>
#include <fstream>

#include "keyboard.hpp"
#include "terminal.hpp"

#define CTRL_KEY(k) ((k) & 0x1f)

class Editor {
    bool saved = false;
    std::string git_tag;
    int rows, cols;
    struct termios orig_termios;
    std::pair<int, int> screen_dimensions;
    std::vector<std::string> buffer;
    int cx = 0, cy = 0; // cursor x and y positions
    int row_offset = 0;
    int col_offset = 0;
    int lines = 0;
    std::string filename;
    double line_percent;
    std::ifstream in;

    public:
    Editor(const std::string& fname);
    ~Editor();

    void scroll();
    void saveToFile();
    void processKey(char key, bool esc);
    void refreshScreen();
    void run();
};
