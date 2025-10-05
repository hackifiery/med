#include <iostream>
#include "editor.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 1){
        cerr << "No file specified!" << endl;
    }
    string fn = argv[1];
    Editor editor(fn);
    editor.run();
    
    return 0;
}
