/*
* Copyright (c) 2025 hackifiery. All rights reserved.
* All code licensed under the MIT License.
*/

#include "editor.hpp"

#include <iostream>
#include <ostream>
#include <unistd.h>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <algorithm>

Editor::Editor(const std::string& fname) : filename(fname) {
    {
        std::ifstream f(filename);
        if (f) saved = true;
    }
    screen_dimensions = getWindowSize();
    rows = screen_dimensions.first;
    cols = screen_dimensions.second;
    tcgetattr(STDIN_FILENO, &orig_termios);
    enableRawMode(orig_termios);
    clearScreen();
    std::ifstream in(filename);
    if (in) {
        std::string line;
        while (getline(in, line)) {
            buffer.push_back(line);
        }
        if (buffer.empty()) buffer.push_back("");
    } else {
        buffer.push_back("");
    }
}

Editor::~Editor() {
    restoreScreen();
    disableRawMode(orig_termios);
    exit(0);
}

void Editor::scroll() {
    if (cy < row_offset) {
        row_offset = cy;
    }
    if (cy >= row_offset + rows - 1) {
        row_offset = cy - rows + 2;
    }
    if (cx < col_offset) {
        col_offset = cx;
    }
    if (cx >= col_offset + cols) {
        col_offset = cx - cols + 1;
    }
}

void Editor::saveToFile(){
    saved = true;
    std::ofstream out(filename);
    for (std::string line : buffer){
        out << line << std::endl;
    }
    out.close();
}

void Editor::processKey(char key, bool esc) {
    if ((long unsigned int) cy < buffer.size()) {
        size_t line_len = buffer[cy].length();
        if (cx > (int)line_len) {
            cx = (int)line_len;
        }
    }
    switch (key){
        case CTRL_KEY('s'):
            saveToFile();
            break;
        case CTRL_KEY('x'):
            delete this;
            return;
        case '\n':
            saved = false;
            if (buffer.empty()) buffer.push_back("");
            buffer.insert(buffer.begin() + cy + 1, buffer[cy].substr(cx));
            buffer[cy] = buffer[cy].substr(0, cx);
            cy++;
            cx = 0;
            break;
        case 127:
            if (cx > 0) {
                buffer[cy].erase(cx - 1, 1);
                cx--;
            }
            else if (cx == 0 && cy > 0){
                std::string current_line = buffer[cy];
                buffer.erase(buffer.begin()+cy);
                cy --;
                cx = static_cast<int>(buffer[cy].length());
                buffer[cy] += current_line;
            }
            break;
    }
    if (esc){
        switch (key) {
            case 'U':
                if (cy > 0){
                    if (cx > (int)buffer[cy-1].size()){
                        cx = (int)buffer[cy-1].size();
                    }
                    cy--;
                }
                break;
            case 'D':
                if (cy < (int)buffer.size() - 1) {
                    if (cx > (int)buffer[cy+1].size()){
                        cx = (int)buffer[cy+1].size();
                    }
                    cy++;
                }
                break;
            case 'L':
                if (cx > 0) cx--;
                break;
            case 'R':
                if (cx < (int)buffer[cy].size()) cx++;
                break;
            case 'H':
                cx = 0;
                break;
            case 'h':
                cx = 0;
                cy = 0;
                break;
            case 'E':
                cx = buffer[cy].length();
                break;
            case 'e':
                cx = buffer.back().length();
                cy = buffer.size()-1;
                break;
        }
    }
    else{
        if (key >= 32 && key <= 126) {
            saved = false;
            buffer[cy].insert(cx, 1, key);
            cx++;
        }
    }
}

void Editor::refreshScreen() {
    lines = buffer.size();
    line_percent = std::round((100.0 * (cy + 1)) / lines);

    int line_num_width = std::to_string(lines).length();

    line_percent = std::round((100.0 * (cy + 1)) / lines);
    scroll();
    std::cout << "\033[H\033[J";
    int lines_to_draw = rows - 1;

    for (int i = 0; i < lines_to_draw; i++) {
        int file_row = row_offset + i;

        if (file_row < (int)buffer.size()) {
            const std::string& line = buffer[file_row];
            int lineno = file_row + 1;
            std::cout <<"\033[90m" << lineno << "\033[0m ";

            if ((int)line.length() > col_offset) {
                std::string visible_part = line.substr(col_offset);

                if ((int)visible_part.length() > cols - line_num_width - 2)
                    visible_part = visible_part.substr(0, cols - line_num_width - 2);

                for (int j = 0; j < line_num_width - (int)std::to_string(i+1).length(); j++){
                    std::cout << " ";
                }
                std::cout << visible_part;
            }

        } else if (file_row == (int)buffer.size()) {
            std::cout << "\033[90m" << std::setw(line_num_width+1) << "~" << "\033[0m";
        }

        std::cout << "\033[K";
        std::cout << "\r\n";
    }

    std::cout << "\033[" << rows << ";1H\033[30;47m";
    for (int i = 0; i < cols; ++i) {
        std::cout << " ";
    }
    std::cout << "\033[" << rows << ";1H";
    std::cout << filename << (saved ? " (saved)" : " (not saved)");

    std::ostringstream status_right;
    status_right << cy + 1 << "," << cx + 1 << " [" << line_percent << "%]";
    std::string right_text = status_right.str();
    int right_start_col = std::max(1, cols - (int)right_text.length() + 1);

    std::cout << "\033[" << rows << ";" << right_start_col << "H" << right_text;
    std::cout << "\033[0m";

    int gutter = line_num_width + 1;

    int screen_cy = cy - row_offset + 1;
    int screen_cx = (cx - col_offset) + gutter + 1;

    std::cout << "\033[" << screen_cy << ";" << screen_cx << "H";
    std::cout.flush();
}

void Editor::run() {
    while (true) {
        refreshScreen();
        std::pair<bool, char> rkey = readKey();
        bool esc = rkey.first;
        char key = rkey.second;
        processKey(key, esc);
    }
}
