#include <string>
#include <iostream>
#include <map>

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
        uint8_t sp;
        uint16_t I;
        uint16_t pc;
        uint16_t opcode;
        uint16_t stack[16];
        uint8_t registers[16];
        uint8_t memory[4096];
        uint8_t delayTimer;
        uint8_t soundTimer;
        map<uint16_t, INSTRUCTION> oplist;

        int loadRom(string romFile);
        void mapInsts();
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
