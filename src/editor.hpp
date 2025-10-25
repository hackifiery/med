#include <iostream>
#include <ostream>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include <cmath>

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
        string line;
        lines = buffer.size();
        while (getline(in, line)) {
            lines++;
        }
        line_percent = round((100.0 * (cy + 1)) / lines);
        scroll();
        cout << "\033[H\033[J"; // clear screen
        int lines_to_draw = rows - 1; // leave last line for status
        
        // print file buffer, starting from the row_offset
        for (int i = 0; i < lines_to_draw; i++) {
            int file_row = row_offset + i;
            
            if (file_row < (int)buffer.size()) {
                const string& line = buffer[file_row];
                
                // Print only the visible part of the line
                if ((int)line.length() > col_offset) {
                    // Substring starts at col_offset
                    string visible_part = line.substr(col_offset);
                    
                    // Truncate to the screen width if it's too long
                    if ((int)visible_part.length() > cols) {
                        visible_part = visible_part.substr(0, cols);
                    }
                    cout << visible_part;
                }
                
            } else if (file_row == (int)buffer.size()) {
                // Print a tilde for lines past the end of the file
                cout << "~"; 
            }

            cout << "\033[K"; // clear rest of the line
            cout << "\r\n";
        }

        // move to status line
        cout << "\033[" << rows << ";1H";  // move cursor to last line
        cout << "\033[K";                  // clear the line

        // print status info
        cout << filename;
        if (saved){
            cout << " (saved)";
        }
        else{
            cout << " (not saved)";
        }
        // print cursor/buffer position
        ostringstream status_right;
        status_right << cy + 1 << "," << cx + 1 
                    << " [" << line_percent << "%]";

        string right_text = status_right.str();
        int right_start_col = max(1, cols - (int)right_text.length() + 1);

        // move cursor to the starting column and print
        cout << "\033[" << right_start_col << "G" << right_text;

        

        // move cursor back to editing position
        // The screen X position is 'cx' minus the 'col_offset', plus 1 (for 1-based indexing)
        int screen_cy = cy - row_offset + 1;
        int screen_cx = cx - col_offset + 1;
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
