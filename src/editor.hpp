#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <vector>
#include <string>
#include <fstream>

#pragma once

#define CTRL_KEY(k) ((k) & 0x1f)

using namespace std;

class Editor {
    struct termios orig_termios;
    vector<string> buffer;
    int cx = 0, cy = 0; // cursor x and y positions
    string filename;
    // ifstream out;

    public:
    Editor(const std::string& fname) : filename(fname) {
        tcgetattr(STDIN_FILENO, &orig_termios);
        enableRawMode();
        ifstream in(filename);
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

    ~Editor() {
        disableRawMode();
    }

    void enableRawMode() {
        struct termios raw = orig_termios;
        raw.c_lflag &= ~(ICANON | ECHO);
        raw.c_iflag &= ~(IXON);  // <--- disable Ctrl-S / Ctrl-Q flow control
        raw.c_cc[VMIN] = 1;
        raw.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }

    void disableRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    }

    void saveToFile(){
        ofstream out(filename);
        for (string line : buffer){
            out << line << endl;
        }
        out.close();
    }

    char readKey() {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) return 0;

        if (c == 27) { // Escape sequence
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) return 27;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) return 27;

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A': return 'U'; // Up
                    case 'B': return 'D'; // Down
                    case 'C': return 'R'; // Right
                    case 'D': return 'L'; // Left
                    default: return '?';
                }
            }
        }
        return c;
    }

    void processKey(char key) {
        switch (key) {
            case CTRL_KEY('q'): // quit
                disableRawMode();
                exit(0);

            case 'U': // up
                if (cy > 0) cy--;
                break;

            case 'D': // down
                if (cy < (int)buffer.size() - 1) {
                    if (cx > (int)buffer[cy+1].size()){
                        cx = (int)buffer[cy+1].size()-1;
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
                    buffer.erase(buffer.begin()+cy);
                    cy --;
                    cx = static_cast<int>(buffer[cy].length());
                }
                break;

            case '\n': // new line
                buffer.insert(buffer.begin() + cy + 1, buffer[cy].substr(cx));
                buffer[cy] = buffer[cy].substr(0, cx);
                cy++;
                cx = 0;
                break;
            
            case CTRL_KEY('s'):
                saveToFile();
                break;

            default: // normal char
                if (key >= 32 && key <= 126) {
                    buffer[cy].insert(cx, 1, key);
                    cx++;
                }
                break;
        }
    }

    void refreshScreen() {
        cout << "\033[H\033[J"; // clear screen
        for (size_t i = 0; i < buffer.size(); i++) {
            cout << buffer[i] << "\r\n";
        }
        // move cursor
        cout << "\033[" << cy + 1 << ";" << cx + 1 << "H";
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
