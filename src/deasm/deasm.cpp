#include <iostream>
#include <fstream>
#include <string>

int main(int argc, char **argv)
{
    if (argc < 3) {
        std::cerr << "DEASM v0.0.1" << std::endl;
        std::cerr << "deasm <input.asm> <output.pd> [add to size]";
        return 1;
    }

    int add_to_size = 0;
    if (argc >= 4) {
        add_to_size = std::stoi(argv[3]);
    }

    

    return 0;
}