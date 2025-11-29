/*
* Copyright (c) 2025 hackifiery. All rights reserved.
* All code licensed under the MIT License.
*/

#include <iostream>
#include <ostream>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <cmath>
#include <sstream>

#include "keyboard.hpp"
#include "terminal.hpp"

#pragma once

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;

class Editor {
    bool saved = false;
    string git_tag;
    int rows, cols;
    struct termios orig_termios;
    pair <int, int> screen_dimensions;
    vector<string> buffer;
    int cx = 0, cy = 0; // cursor x and y positions
    int row_offset = 0;
    int col_offset = 0;
    int lines = 0;
    string filename;
    double line_percent;
    ifstream in;
    // ifstream out;

    public:
    Editor(const string& fname) : filename(fname) {
        if (filesystem::exists(filename)){
            saved = true;
        }
        screen_dimensions = getWindowSize();
        rows = screen_dimensions.first;
        cols = screen_dimensions.second;
        tcgetattr(STDIN_FILENO, &orig_termios);
        enableRawMode(orig_termios);
        clearScreen();
        ifstream in(filename);
        if (in) {
            string line;
            while (getline(in, line)) {
                buffer.push_back(line);
            }
            if (buffer.empty()) buffer.push_back("");
        } else {
            buffer.push_back("");
        }
    }

    ~Editor() {
        restoreScreen();
        disableRawMode(orig_termios);
        exit(0);
        // cout << endl;
    }

    void scroll() {
        // Vertical scrolling
        if (cy < row_offset) {
            row_offset = cy;
        }
        // Use rows-1 because the last row is for status bar
        if (cy >= row_offset + rows - 1) { 
            row_offset = cy - rows + 2; // +2 to keep the cursor one line above the status bar
        }
        if (cx < col_offset) {
            col_offset = cx;
        }
        // Use cols-1 because the last column is for the cursor position
        if (cx >= col_offset + cols) {
            col_offset = cx - cols + 1;
        }
    }

    void saveToFile(){
        saved = true;
        ofstream out(filename);
        for (string line : buffer){
            out << line << endl;
        }
        out.close();
    }
    

    void processKey(char key, bool esc) {
        if ((long unsigned int) /* prevent comparison warning */ cy < buffer.size()) {
            size_t line_len = buffer[cy].length();
            if (cx > (int)line_len) {
                cx = (int)line_len;
            }
        }
        switch (key){
            case CTRL_KEY('s'):
                saveToFile();
                break;
            case CTRL_KEY('x'): // quit
                delete this; // no, don't actually delete this line of code
                return;
            case '\n': // new line
                saved = false;
                if (buffer.empty()) buffer.push_back("");

                buffer.insert(buffer.begin() + cy + 1, buffer[cy].substr(cx));
                buffer[cy] = buffer[cy].substr(0, cx);
                cy++;
                cx = 0; 
                break;
            case 127: // backspace
                if (cx > 0) {
                    buffer[cy].erase(cx - 1, 1);
                    cx--;
                }
                else if (cx == 0 && cy > 0){
                    // Merge current line into the previous one
                    string current_line = buffer[cy];
                    buffer.erase(buffer.begin()+cy);
                    cy --;
                    // cx should be set to the end of the line *before* the merge
                    cx = static_cast<int>(buffer[cy].length()); 
                    buffer[cy] += current_line; // Now append
                }
                break;
        }
        if (esc){
            switch (key) {
                case 'U': // up
                    if (cy > 0){
                        if (cx > (int)buffer[cy-1].size()){
                            cx = (int)buffer[cy-1].size();
                        }
                        cy--;
                    }
                    break;

                case 'D': // down
                    if (cy < (int)buffer.size() - 1) {
                        if (cx > (int)buffer[cy+1].size()){
                            cx = (int)buffer[cy+1].size();
                        }
                        cy++;
                    }
                    break;

                case 'L': // left
                    if (cx > 0) cx--;
                    break;

                case 'R': // right
                    if (cx < (int)buffer[cy].size()) cx++;
                    break;

                case 'H': // home key
                    cx = 0;
                    break;
                
                case 'h': // ctrl+home
                    cx = 0;
                    cy = 0;
                    break;
                
                case 'E': // end key
                    cx = buffer[cy].length();
                    break;
                
                case 'e': // ctrl+end
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
    void refreshScreen() {
        
        lines = buffer.size();
        line_percent = round((100.0 * (cy + 1)) / lines);

        int line_num_width = std::to_string(lines).length();

        line_percent = round((100.0 * (cy + 1)) / lines);
        scroll();
        cout << "\033[H\033[J"; // clear screen
        int lines_to_draw = rows - 1; // leave last line for status
        
        // print file buffer, starting from the row_offset
        for (int i = 0; i < lines_to_draw; i++) {
            int file_row = row_offset + i;
            
            if (file_row < (int)buffer.size()) {
                const string& line = buffer[file_row];
                int lineno = file_row + 1;
                // print padded line number + separator
                cout << std::setw(line_num_width) << "\033[90m" << lineno << "\033[0m ";

                // Print only visible part of text
                if ((int)line.length() > col_offset) {
                    string visible_part = line.substr(col_offset);

                    if ((int)visible_part.length() > cols - line_num_width - 3)
                        visible_part = visible_part.substr(0, cols - line_num_width - 3);

                    cout << visible_part;
                }
                
            } else if (file_row == (int)buffer.size()) {
                // Print a tilde for lines past the end of the file
                cout << "~"; 
            }

            cout << "\033[K"; // clear rest of the line
            cout << "\r\n";
        }

        // Move to status line and set color
		cout << "\033[" << rows << ";1H\033[30;47m";

		// Fill the entire line with spaces
		for (int i = 0; i < cols; ++i) {
		    cout << " ";
		}

		// Move back to start of line
		cout << "\033[" << rows << ";1H";

		// Print left-aligned status
		cout << filename << (saved ? " (saved)" : " (not saved)");

		// Prepare right-aligned status
		ostringstream status_right;
		status_right << cy + 1 << "," << cx + 1 << " [" << line_percent << "%]";
		string right_text = status_right.str();
		int right_start_col = max(1, cols - (int)right_text.length() + 1);
		
		// Move to right-aligned position and print
		cout << "\033[" << rows << ";" << right_start_col << "H" << right_text;

		// Reset formatting
		cout << "\033[0m";

		// Move cursor back to editing position
		// line_num_width = std::to_string(lines).length();
        int gutter = line_num_width + 1; // width of "NNN "

        int screen_cy = cy - row_offset + 1;
        int screen_cx = (cx - col_offset) + gutter + 1;

        cout << "\033[" << screen_cy << ";" << screen_cx << "H";
		cout.flush();
    }

    void run() {
            while (true) {
                refreshScreen();
                pair<bool, char> rkey = readKey();
                bool esc = rkey.first;
                char key = rkey.second;
                processKey(key, esc);
        }
    }
};
