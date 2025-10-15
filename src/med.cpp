#include <iostream>
#include "editor.hpp"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc <= 1){
        cerr << "No file specified!" << endl;
        // return 1;
    }
    else if (argc == 2){
        if (string(argv[1]) == "--version" || string(argv[1]) == "-v"){
            cout << "med version " << GIT_TAG << endl;
            return 0;
        }
        string fn = argv[1];
        Editor editor(fn);
        editor.run();
        
        return 0;
    }
    return 1;
}
