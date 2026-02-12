/*
* Copyright (c) 2025 hackifiery. All rights reserved.
* All code licensed under the MIT License.
*/

#include <iostream>
#include <sys/stat.h>
#include "editor.hpp"
#if defined(PROJECT_VERSION_STRING_CMAKE) && defined(PROJECT_VERSION_STRING_GIT)
#include "version.hpp"
#else
#define PROJECT_VERSION_STRING_GIT "unknown"
#define PROJECT_VERSION_STRING_CMAKE "unknown"
#endif

using namespace std;
string git_tag;

int main(int argc, char* argv[]) {
    if (argc <= 1){
        cerr << "No file specified!" << endl;
        // return 1;
    }
    
    else if (argc == 2){
        struct stat st;
        if (stat(".git", &st) == 0 && S_ISDIR(st.st_mode)){
            git_tag = PROJECT_VERSION_STRING_CMAKE;
        } else {
            git_tag = PROJECT_VERSION_STRING_GIT;
        }
        if (string(argv[1]) == "--version" || string(argv[1]) == "-v"){
            cout << "med version " << git_tag << endl;
            return 0;
        }
        string fn = argv[1];
        Editor editor(fn);
        editor.run();
        
        return 0;
    }
    return 1;
}
