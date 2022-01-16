#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <ctime>
#include <cstdlib>
#include <chrono>

#include "ioInterface.h"

using namespace std;

class Chip8;

struct INSTRUCTION {
    string name;
    void (Chip8::*execute)();
};

class Chip8 {
    public:
        Chip8(int scale, int delay, string romFile);
        void run();
        INSTRUCTION decode();

    private:
        // Hardware to emulate
        uint8_t sp;
        uint16_t I;
        uint16_t pc;
        uint16_t opcode;
        uint16_t stack[16];
        uint8_t keys[16];
        uint8_t registers[16];
        uint8_t memory[4096];
        uint32_t videoMemory[32*64] = {0};
        uint8_t delayTimer;
        uint8_t soundTimer;
        map<uint16_t, INSTRUCTION> oplist;
        IoInterface ioItf;

        int delay;

        // General private functions
        void mapInsts();
        int loadRom(string romFile);
        void cycle();
        
        // Fonts for hex numbers
        uint8_t font[16*5] = {
		    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		    0x20, 0x60, 0x20, 0x20, 0x70, // 1
		    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
	    	0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	    };

        // Instruction functions
        void CLS(); void RET(); void JMP(); void CALL();
        void SEI(); void SNEI(); void SE(); void LD();
        void ADDI(); void LDR(); void OR(); void AND();
        void XOR(); void ADD(); void SUB(); void SHR();
        void SUBN(); void SHL(); void SNE(); void LDI();
        void JP0(); void RND(); void DRW(); void SKP();
        void SKNP(); void LDDT(); void LDK(); void SDT();
        void SST(); void ADDRI(); void LDIS(); void SBCD();
        void SRI(); void LIR();

};
