#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>

#include "keyboard.hpp"
#include "terminal.hpp"

#pragma once

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;

class Editor {
    bool saved = false;
    string git_hash = GIT_COMMIT_HASH;
    string git_branch = GIT_BRANCH;
    string git_tag = GIT_TAG;
    int rows, cols;
    struct termios orig_termios;
    vector<string> buffer;
    int cx = 0, cy = 0; // cursor x and y positions
    int row_offset = 0;
    string filename;
    // ifstream out;

    public:
    Editor(const string& fname) : filename(fname) {
        getWindowSize(rows, cols);
        if (filesystem::exists(filename)){
            saved = true;
        }
        tcgetattr(STDIN_FILENO, &orig_termios);
        enableRawMode(orig_termios);
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
        disableRawMode(orig_termios);
        // system("clear");
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
        // TODO: horizontal scrolling
    }

    void saveToFile(){
        saved = true;
        ofstream out(filename);
        for (string line : buffer){
            out << line << endl;
        }
        out.close();
    }
    

    void processKey(char key) {
        switch (key) {
            case CTRL_KEY('q'): // quit
                disableRawMode(orig_termios);
                system("clear");
                exit(0);

            case 'U': // up
                if (cy > 0) cy--;
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

            case '\n': // new line
                saved = false;
                // Check for empty buffer which can happen if you load an empty file
                if (buffer.empty()) buffer.push_back("");

                buffer.insert(buffer.begin() + cy + 1, buffer[cy].substr(cx));
                buffer[cy] = buffer[cy].substr(0, cx);
                cy++;
                cx = 0; 
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
                cy = buffer.size();
                break;

            case CTRL_KEY('s'):
                saveToFile();
                break;

            default: // normal char
                if (key >= 32 && key <= 126) {
                    saved = false;
                    buffer[cy].insert(cx, 1, key);
                    cx++;
                }
                break;
        }
    }
    void refreshScreen() {
        scroll();
        cout << "\033[H\033[J"; // clear screen
        int lines_to_draw = rows - 1; // leave last line for status
        // print file buffer, starting from the row_offset
        for (int i = 0; i < lines_to_draw; i++) {
            int file_row = row_offset + i;
            
            if (file_row < (int)buffer.size()) {
                cout << buffer[file_row];
            } else if (file_row == (int)buffer.size()) {
                // Print a tilde for lines past the end of the file, common in terminal editors
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
        cout << " | L:" << cy + 1 << ", C:" << cx + 1;
        

        // move cursor back to editing position
        // The visual Y position is 'cy' minus the 'row_offset', plus 1 (for 1-based indexing)
        int screen_cy = cy - row_offset + 1;
        cout << "\033[" << screen_cy << ";" << cx + 1 << "H";
        cout.flush();
    }

    void run() {
            while (true) {
                refreshScreen();
                char key = readKey();
                processKey(key);
        }
    }
};
