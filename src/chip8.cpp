#include "chip8.h"

Chip8::Chip8(int scale, int delay, string romFile) {
    // Load ROM file
    if (loadRom(romFile)) {
        exit(EXIT_FAILURE);
    }

    // Populate the instruction map used in decoding
    mapInsts();
}

int Chip8::loadRom(string romFile) {

    return 0;
}

void Chip8::run() {
    // Cycle
}

void Chip8::mapInsts() {
    // Builds a map to decode any legal instruction to its execution function
    // Because what even is memory efficiency anyways
    //
    // Instruction decoding from Cowgod's chip-8 technical reference - see read me for link
    //
    // nnn or addr - A 12-bit value, the lowest 12 bits of the instruction
    // n or nibble - A 4-bit value, the lowest 4 bits of the instruction
    // x - A 4-bit value, the lower 4 bits of the high byte of the instruction
    // y - A 4-bit value, the upper 4 bits of the low byte of the instruction
    // kk or byte - An 8-bit value, the lowest 8 bits of the instruction
    //
    oplist[0x00E0u].name = "CLS";
    oplist[0x00E0u].execute = &Chip8::CLS;

    oplist[0x00EEu].name = "RET";
    oplist[0x00EEu].execute = &Chip8::RET;

    // 1nnn
    for (int i=0x1000u; i < 0x2000u; i++) {
        oplist[i].name = "JP addr";
        oplist[i].execute = &Chip8::JMP;
    }

    // 2nnn
    for (int i=0x2000u; i < 0x3000u; i++) {
        oplist[i].name = "CALL addr";
        oplist[i].execute = &Chip8::CALL;
    }

    // 3xkk
    for (int i=0x3000u; i < 0x4000u; i++) {
        oplist[i].name = "Skip Equal Vx, byte";
        oplist[i].execute = &Chip8::SEI;
    }

    // 4xkk
    for (int i=0x4000u; i < 0x5000u; i++) {
        oplist[i].name = "Skip Not Equal Vx, byte";
        oplist[i].execute = &Chip8::SNEI;
    }

    // 5xy0
    for (int i=0x5000u; i < 0x6000u; i+=0x0Fu) {
        oplist[i].name = "Skip Equal Vx, Vy";
        oplist[i].execute = &Chip8::SE;
    }

    // 6xkk
    for (int i=0x6000u; i < 0x7000u; i++) {
        oplist[i].name = "LD Vx, byte";
        oplist[i].execute = &Chip8::LD;
    }

    // 7xkk
    for (int i=0x7000u; i < 0x8000u; i++) {
        oplist[i].name = "ADD Vx, byte";
        oplist[i].execute = &Chip8::ADDI;
    }

    // 8xy0
    for (int i=0x8000u; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "LD Vx, Vy";
        oplist[i].execute = &Chip8::LDR;
    }

    // 8xy1
    for (int i=0x8001u; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "OR Vx, Vy";
        oplist[i].execute = &Chip8::OR;
    }

    // 8xy2
    for (int i=0x8002u; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "AND Vx, Vy";
        oplist[i].execute = &Chip8::AND;
    }

    // 8xy3
    for (int i=0x8003u; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "XOR Vx, Vy";
        oplist[i].execute = &Chip8::XOR;
    }

    // 8xy4
    for (int i=0x8004u; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "ADD registers, Vx, Vy";
        oplist[i].execute = &Chip8::ADD;
    }

    // 8xy5
    for (int i=0x8005u; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "SUB Vx, Vy";
        oplist[i].execute = &Chip8::SUB;
    }

    // 8xy6
    for (int i=0x8006u; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "SHR Vx, Vy";
        oplist[i].execute = &Chip8::SHR;
    }

    // 8xy7
    for (int i=0x8007u; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "SUBN Vx{, Vy}";
        oplist[i].execute = &Chip8::SUBN;
    }

    // 8xyE
    for (int i=0x800Eu; i < 0x9000u; i+=0x0Fu) {
        oplist[i].name = "SHL Vx{, Vy}";
        oplist[i].execute = &Chip8::SHL;
    }

    // 9xy0
    for (int i=0x9000u; i < 0xA000u; i+=0x0Fu) {
        oplist[i].name = "SNE Vx, Vy";
        oplist[i].execute = &Chip8::SNE;
    }

    // Annn
    for (int i=0xA000u; i < 0xB000u; i++) {
        oplist[i].name = "LD I, addr";
        oplist[i].execute = &Chip8::LDI;
    }

    // Bnnn
    for (int i=0xB000u; i < 0xC000u; i++) {
        oplist[i].name = "JP V0, addr";
        oplist[i].execute = &Chip8::JP0;
    }

    // Cxkk
    for (int i=0xC000u; i < 0xD000u; i++) {
        oplist[i].name = "RND Vx, byte";
        oplist[i].execute = &Chip8::RND;
    }

    // Dxyn
    for (int i=0xD000u; i < 0xE000u; i++) {
        oplist[i].name = "DRW Vx, Vy, nibble";
        oplist[i].execute = &Chip8::DRW;
    }

    // Ex9E
    for (int i=0xE09Eu; i <= 0xEF9Eu; i+=0x0100) {
        oplist[i].name = "SKP Vx";
        oplist[i].execute = &Chip8::SKP;
    }

    // ExA1
    for (int i=0xE0A1u; i <= 0xEFA1u; i+=0x0100) {
        oplist[i].name = "SKNP Vx";
        oplist[i].execute = &Chip8::SKNP;
    }

    // Fx07
    for (int i=0xF007u; i <= 0xFF07u; i+=0x0100) {
        oplist[i].name = "LD Vx, DT";
        oplist[i].execute = &Chip8::LDDT;
    }

    // Fx0A
    for (int i=0xF00Au; i <= 0xFF0Au; i+=0x0100) {
        oplist[i].name = "LD Vx K";
        oplist[i].execute = &Chip8::LDK;
    }

    // Fx15
    for (int i=0xF015u; i <= 0xFF15u; i+=0x0100) {
        oplist[i].name = "LD DT, Vx";
        oplist[i].execute = &Chip8::SDT;
    }

    // Fx18
    for (int i=0xF018u; i <= 0xFF18u; i+=0x0100) {
        oplist[i].name = "LD ST, Vx";
        oplist[i].execute = &Chip8::SST;
    }

    // Fx1E
    for (int i=0xF01Eu; i <= 0xFF1Eu; i+=0x0100) {
        oplist[i].name = "ADD I, Vx";
        oplist[i].execute = &Chip8::ADDRI;
    }

    // Fx29
    for (int i=0xF029u; i <= 0xFF29u; i+=0x0100) {
        oplist[i].name = "LD F, Vx"; 
        oplist[i].execute = &Chip8::LDIS;
    }

    // Fx33
    for (int i=0xF033u; i <= 0xFF33u; i+=0x0100) {
        oplist[i].name = "LD B, Vx";
        oplist[i].execute = &Chip8::SBCD;
    }

    // Fx55
    for (int i=0xF055u; i <= 0xFF55u; i+=0x0100) {
        oplist[i].name = "LD [I], Vx";
        oplist[i].execute = &Chip8::SRI;
    }

    // Fx65
    for (int i=0xF065u; i <= 0xFF65u; i+=0x0100) {
        oplist[i].name = "LD Vx, [I]";
        oplist[i].execute = &Chip8::LIR;
    }
}

void Chip8::CLS() { } void Chip8::RET() { } void Chip8::JMP() { } void Chip8::CALL() { }
void Chip8::SEI() { } void Chip8::SNEI() { } void Chip8::SE() { } void Chip8::LD() { }
void Chip8::ADDI() { } void Chip8::LDR() { } void Chip8::OR() { } void Chip8::AND() { }
void Chip8::XOR() { } void Chip8::ADD() { } void Chip8::SUB() { } void Chip8::SHR() { }
void Chip8::SUBN() { } void Chip8::SHL() { } void Chip8::SNE() { } void Chip8::LDI() { }
void Chip8::JP0() { } void Chip8::RND() { } void Chip8::DRW() { } void Chip8::SKP() { }
void Chip8::SKNP() { } void Chip8::LDDT() { } void Chip8::LDK() { } void Chip8::SDT() { }
void Chip8::SST() { } void Chip8::ADDRI() { } void Chip8::LDIS() { } void Chip8::SBCD() { }
void Chip8::SRI() { } void Chip8::LIR() { }
