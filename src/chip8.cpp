#include "chip8.h"

Chip8::Chip8(int scale, int delay, string romFile) : ioItf("Chip8 Emulator", scale * 64, scale * 32, 64, 32) {
    
    this->delay = delay;
    srand(time(nullptr));

    // Load ROM file
    if (loadRom(romFile)) {
        exit(EXIT_FAILURE);
    }

    // Point PC to start of ROM
    pc = 0x200;

    // Populate the instruction map used in decoding
    mapInsts();

    // Load font to memory
    for (uint16_t i = 0; i < 16; i++) {
		memory[0x50u + i] = font[i];
	}
}

int Chip8::loadRom(string romFileName) {
    // Opens rom file then reads to a buffer before writing to chip8 memory
    ifstream romFile(romFileName, ios::binary | ios::ate);

    if (romFile.is_open()) {
        streampos fileSize = romFile.tellg();
        char* buf = (char*) malloc(fileSize * sizeof(char));
        romFile.seekg(0, ios::beg);
		romFile.read(buf, fileSize);
		romFile.close();

		for (long long i = 0; i < fileSize; i++)
		{
			memory[0x200 + i] = buf[i];
		}

        free(buf);
    }

    return 0;
}

void Chip8::run() {
    bool quit = false;
    int videoPitch = sizeof(videoMemory[0]) * 64;
    auto lastCycleTime = chrono::high_resolution_clock::now();

    while (!quit) {
        quit = ioItf.getInput(keys);

        auto currentTime = chrono::high_resolution_clock::now();
		float dt = chrono::duration<float, chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > delay) {
            lastCycleTime = currentTime;
            cycle();
            ioItf.drawFrame(videoMemory, videoPitch);
        }
    }
}

void Chip8::cycle() {
    // Read next inst (2 bytes) and increment pc
    opcode = (uint16_t) memory[pc++];
    opcode <<= 8u;
    opcode |= (uint16_t) memory[pc++];

    // Execute inst
    (this->*oplist[opcode].execute)();
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

void Chip8::CLS() {
    // Clear screen by reseting video memory
    memset(videoMemory, 0, sizeof(videoMemory));
}

void Chip8::RET() {
    pc = memory[sp];
    sp--;
}

void Chip8::JMP() {
    pc = opcode & 0x0FFFu;
}

void Chip8::CALL() {
    sp++;
    memory[sp] = pc;
    pc = opcode & 0x0FFFu;
}

void Chip8::SEI() {
    uint8_t Vx = registers[(opcode & 0x0F00u) >> 8u];
    uint8_t immediate = opcode & 0x00FF;

    if (Vx == immediate)
        pc += 2;
}

void Chip8::SNEI() {
    uint8_t Vx = registers[(opcode & 0x0F00u) >> 8u];
    uint8_t immediate = opcode & 0x00FF;

    if (Vx != immediate)
        pc += 2;
}

void Chip8::SE() {
    uint8_t Vx = registers[(opcode & 0x0F00u) >> 8u];
    uint8_t Vy = registers[(opcode & 0x00F0u) >> 4u];

    if (Vx == Vy)
        pc += 2;
}

void Chip8::LD() {
    registers[(opcode & 0x0F00u) >> 8u] = opcode & 0x00FFu;
}

void Chip8::ADDI() {
    registers[(opcode & 0x0F00u) >> 8u] += opcode & 0x00FFu;
}

void Chip8::LDR() {
    registers[(opcode & 0x0F00u) >> 8u] = registers[(opcode & 0x00F0u) >> 4u];
}

void Chip8::OR() {
    registers[(opcode & 0x0F00u) >> 8u] |= registers[(opcode & 0x00F0u) >> 4u];
}

void Chip8::AND() {
    registers[(opcode & 0x0F00u) >> 8u] &= registers[(opcode & 0x00F0u) >> 4u];
}

void Chip8::XOR() { 
    registers[(opcode & 0x0F00u) >> 8u] ^= registers[(opcode & 0x00F0u) >> 4u];
}

void Chip8::ADD() {
    // Do addition
    uint8_t VxIdx = (opcode & 0x0F00u) >> 8u;
    uint16_t res = (uint16_t) registers[VxIdx] + (uint16_t) registers[(opcode & 0x00F0u) >> 4u];

    // Calculate and write carry
    registers[0xF] = res > 255u ? 1 : 0;

    // Convert to 8 bit int and write
    registers[VxIdx] = (uint8_t) (res & 0xFFu);
}
void Chip8::SUB() {
    uint8_t VxIdx = (opcode & 0x0F00u) >> 8u;
    uint8_t VyIdx = (opcode & 0x00F0u) >> 4u;

    // Calculate and write carry
    registers[0xF] = registers[VxIdx] > registers[VyIdx] ? 1 : 0;

    // Do subtraction
    registers[VxIdx] -= registers[VyIdx];
}

void Chip8::SHR() {
    uint8_t VxIdx = (opcode & 0x0F00u) >> 8u;

    // Set shifted bit (LSB) in VF
    registers[0xF] = registers[VxIdx] & 0x01u;

    // Do the shift
    registers[VxIdx] >>= 1;
}

void Chip8::SUBN() {
    uint8_t VxIdx = (opcode & 0x0F00u) >> 8u;
    uint8_t VyIdx = (opcode & 0x00F0u) >> 4u;

    // Calculate and write carry
    registers[0xF] = registers[VyIdx] > registers[VxIdx] ? 1 : 0;

    // Do subtraction
    registers[VxIdx] -= registers[VyIdx];
}

void Chip8::SHL() {
    uint8_t VxIdx = (opcode & 0x0F00u) >> 8u;

    // Set shifted bit (MSB) in VF
    registers[0xF] = (registers[VxIdx] & 0x80u) >> 7u;

    // Do the shift
    registers[VxIdx] <<= 1;
}

void Chip8::SNE() {
    if (registers[(opcode & 0x0F00u) >> 8u] != registers[(opcode & 0x00F0u) >> 4u])
        pc += 2;
}

void Chip8::LDI() {
    I = opcode & 0x0FFFu;
}

void Chip8::JP0() {
    pc = registers[0x0] + (opcode & 0x0FFFu);
}

void Chip8::RND() {
    registers[(opcode & 0x0F00u) >> 8u] = (rand()/((RAND_MAX + 1u)/255)) & (opcode & 0x00FFu);
}

void Chip8::DRW() {
	uint8_t height = opcode & 0x000Fu;

    // Check if sprite goes beyond screen boundaries (and wrap around)
	uint8_t xPos = registers[(opcode & 0x0F00u) >> 8u] % 64;
	uint8_t yPos = registers[(opcode & 0x00F0u) >> 4u] % 32;
    
    // Reset flag
	registers[0xF] = 0;

    // 
	for (unsigned int row = 0; row < height; ++row)
	{
		uint8_t spriteByte = memory[I + row];

		for (unsigned int col = 0; col < 8; ++col)
		{
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &videoMemory[(yPos + row) * 64 + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel)
			{
                // Collison since both are on
				if (*screenPixel == 0xFFFFFFFF)
					registers[0xF] = 1;

				// XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

void Chip8::SKP() {
    if (keys[registers[(opcode & 0x0F00u) >> 8u]])
        pc += 2;
}

void Chip8::SKNP() {
    if (!keys[registers[(opcode & 0x0F00u) >> 8u]])
        pc += 2;
}

void Chip8::LDDT() {
    registers[(opcode & 0x0F00u) >> 8u] = delayTimer;
}

void Chip8::LDK() {
    bool pressed = false;
    for (int i=0; i < 16; i++) {
        if (keys[i]) {
            pressed = true;
            registers[(opcode & 0x0F00u) >> 8u] = i;
        }
    }

    // If no key press then cycle again and execute this same inst to update key presses
    if (pressed)
        pc -= 2;
}

void Chip8::SDT() {
    delayTimer = registers[(opcode & 0x0F00u) >> 8u];
}

void Chip8::SST() {
    soundTimer = registers[(opcode & 0x0F00u) >> 8u];
}

void Chip8::ADDRI() {
    I += registers[(opcode & 0x0F00u) >> 8u];
}

void Chip8::LDIS() {
	I = 0x50u + (5 * registers[(opcode & 0x0F00u) >> 8u]);
}

void Chip8::SBCD() {
	uint8_t value = registers[(opcode & 0x0F00u) >> 8u];

	// Ones
	memory[I + 2] = value % 10;
	value /= 10;

	// Tens
	memory[I + 1] = value % 10;
	value /= 10;

	// Hundreds
	memory[I] = value % 10;
}

void Chip8::SRI() {
    uint8_t VxIdx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= VxIdx; ++i)
	{
		memory[I + i] = registers[i];
	}
}

void Chip8::LIR() {
    uint8_t VxIdx = (opcode & 0x0F00u) >> 8u;

	for (uint8_t i = 0; i <= VxIdx; ++i)
	{
		registers[i] = memory[I + i];
	}
}
