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
private:
public:
    bool saved;
    std::string git_tag;
    int rows, cols;
    struct termios orig_termios;
    std::pair<int, int> screen_dimensions;
    std::vector<std::string> buffer;
    int cx, cy; // cursor x and y positions
    int row_offset;
    int col_offset;
    int lines;
    std::string filename;
    double line_percent;
    std::ifstream in;
	
    Editor(const std::string& fname);
    ~Editor();

    void scroll();
    void saveToFile();
    void processKey(char key, bool esc);
    void refreshScreen();
    void run();
};
