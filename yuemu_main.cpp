#include "yuemu.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Please provide the program file path as an argument\n";
        return 1;
    }

    std::string fpath(argv[1]);
    Yuemu yuemu(fpath);
    return 0;
}
