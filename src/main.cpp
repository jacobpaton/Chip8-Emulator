#include "chip8.h"
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 4) {
        cout << "Usage: ./chip8 scale delay romFile" << endl;
        exit(EXIT_FAILURE);
    }

    Chip8 emu(stoi(argv[1]), stoi(argv[2]), argv[3]);
    emu.run();

    return 0;
}
