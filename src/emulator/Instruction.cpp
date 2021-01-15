#include "Instruction.hpp"

#include "Chip8.hpp"

#include <iostream>
#include <cstdio>
#include <bitset>

namespace fish {

//Instructions referenced from Cowgod's Chip-8 Reference
//
//Note: 
//Some instructions when setting the Program Counter will subtract 2,
//this is because the PC is increased by 2 right after in the Chip8's
//cycle function.

//This function would be the SYS Addr instruction which is ignored by modern interpreters
//so I'm using it as a nop instruction
//0nnn - SYS addr
void NOP(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    return;
}

//00E0 - CLS
void CLS(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Clear the screen to zeros
    memset(screen, 0, CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT);
}

//00EE - RET
void RET(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Set PC to address at the top of the stack
    regs.PC = stack[regs.SP];

    //Decrement the stack pointer if it is greater than 1
    regs.SP -= regs.SP > 0 ? 1 : 0;
}

//1nnn - JP addr
void JP_1(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Set PC to the address in the lower 12 bits of the instruction, aka operands
    regs.PC = operands - 2;
}

//2nnn - CALL addr
void CALL(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Increment the stack pointer TODO: Add some bounds check
    regs.SP += 1;

    //Set the top of the stack to the current address, or PC
    stack[regs.SP] = regs.PC;

    //Jump to the address pointed to by operands
    regs.PC = operands - 2;
}

//3xnn - SE Vx, byte
void SE_3(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Check if Vx is equal to nn and if so skip the next instruction
    uint8_t x = operands >> 8;
    uint8_t nn = operands & 0xff;
    
    if(regs.V[x] == nn) regs.PC += 2;   
}

//4xnn - SNE Vx, byte
void SNE_4(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stac, const bool *keys) {
    //Check if Vx is not equal to nn and if so skip the next instruction
    uint8_t x = operands >> 8;
    uint8_t nn = operands & 0xff;
    
    if(regs.V[x] != nn) regs.PC += 2;   
}

//5xy0 - SE Vx, Vy
void SE_5(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Check if Vx is equal to Vy and if so skip the next instruction
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    
    if(regs.V[x] == regs.V[y]) regs.PC += 2;   
}

//6xnn - LD Vx, byte
void LD_6(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Put nn into register Vx
    uint8_t x = operands >> 8;
    uint8_t nn = operands & 0xff;
    regs.V[x] = nn;
}

//7xnn - ADD Vx, byte
void ADD_7(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Add nn to Vx and store it back in Vx
    uint8_t x = operands >> 8;
    uint8_t nn = operands & 0xff;
    regs.V[x] += nn;
}

//8xy0 - LD Vx, Vy
void LD_8(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Stores the value from Vy into Vx
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    regs.V[x] = regs.V[y];
}

//8xy1 - OR Vx, Vy
void OR(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Performs a bitwise OR on Vx and Vy and stores the result into Vx
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    regs.V[x] |= regs.V[y];
}

//8xy2 - AND Vx, Vy
void AND(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Performs a bitwise AND on Vx and Vy and stores the result into Vx
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    regs.V[x] &= regs.V[y];
}

//8xy3 - XOR Vx, Vy
void XOR(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Performs a bitwise XOR on Vx and Vy and stores the result into Vx
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    regs.V[x] ^= regs.V[y];
}

//8xy4 - ADD Vx, Vy
void ADD_8(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Adds the value in Vx to the value in Vy and if the result if greater than a byte can store
    //(> 255), then VF is set to 1, otherwise 0.
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    uint16_t sum = regs.V[x] + regs.V[y];
    regs.VF = sum > 255;
    regs.V[x] = sum & 0xff;
}

//8xy5 - SUB Vx, Vy
void SUB(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Subtracts the value in Vy from Vx and stores it into Vx, if Vx > Vy then VF is set to
    //1, otherwise 0.
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    uint8_t dif = regs.V[x] - regs.V[y];
    regs.VF = regs.V[x] > regs.V[y];
    regs.V[x] = dif;
}

//8xy6 - SHR Vx {, Vy}
void SHR(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //If the least-significant bit is set to 1 then VF is set to 1, otherwise 0.
    //Then Vx is shifted to the right by one / divided by 2
    uint8_t x = operands >> 8;
    //y is unused
    regs.VF = regs.V[x] & 0x1;
    regs.V[x] = regs.V[x] >> 1;
}

//8xy7 - SUBN Vx, Vy
void SUBN(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Subtracts the value in Vx from the value in Vy and stores it into Vx, if Vy > Vx then VF is set to
    //0, otherwise 1 (NOT borrow).
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    regs.VF = regs.V[y] <= regs.V[x];
    regs.V[x] = regs.V[y] - regs.V[x];
}

//8xyE - SHL Vx {, Vy}
void SHL(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //If the most-significant bit is set to 1 then VF is set to 1, otherwise 0.
    //Then Vx is shifted to the left by one / multiplied by 2
    uint8_t x = operands >> 8;
    //y is unused
    regs.VF = regs.V[x] & 0x80; //0b10000000
    regs.V[x] = regs.V[x] << 1;
}

//9xy0 - SNE Vx, Vy
void SNE_9(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Skip the next instruction if Vx is not equal to Vy
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;

    if(regs.V[x] != regs.V[y]) regs.PC += 2;
}

//Annn - LD I, addr
void LD_A(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //The value of register I is set to nnn
    regs.I = operands;
}

//Bnnn - JP V0, addr
void JP_B(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Jump to / set PC to, nnn + V0
    regs.PC = (operands + regs.V[0]) - 2;
}

//Cxnn - RND Vx, byt
void RND(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Generate a random number and AND it with nn, then stores it in Vx.
    uint8_t x = operands >> 8;
    uint8_t nn = operands & 0xff;
    regs.V[x] = (rand() % 255) & nn; //TODO: Use a better random number generator than rand()
}

//Dxyn - DRW Vx, Vy, nibble
void DRW(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Read an n byte sprite in from memory starting at I, then XOR them onto the screen
    //at (Vx, Vy)
    uint8_t x = operands >> 8;
    uint8_t y = (operands >> 4) & 0xf;
    uint8_t n = operands & 0xf;
    regs.VF = 0;

    for(int i = 0; i < n; i++) {
        uint8_t sprite_line = mem[regs.I + i];

        for(int j = 0; j < 8; j++) {
            if(sprite_line & (0x80 >> j)) {
                //Screen wrapping
                size_t pos = ((regs.V[x] + j) + (regs.V[y] + i) * CHIP8_SCREEN_WIDTH) % CHIP8_SCREEN_PIXELS;

                //XOR the pixel and check for collision; Performing a NOT when the pixel is guaranteed to be 1
                //is effectivally the same as an XOR.
                regs.VF |= screen[pos] & 1;
                screen[pos] = ~screen[pos];
            }
        }
    }
}

//Ex9E - SKP Vx
void SKP(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Skip the next instruction if the key with the value in Vx is currently down
    uint8_t x = operands >> 8;
    regs.PC += keys[regs.V[x]] ? 2 : 0;
}

//ExA1 - SKNP Vx
void SKNP(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Skip the next instruction if the key with the value in Vx is currently up
    uint8_t x = operands >> 8;
    regs.PC += !keys[regs.V[x]] ? 2 : 0;
}

//Fx07 - LD Vx, DT
void LD_F07(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Set Vx to the value in the Delay Timer
    uint8_t x = operands >> 8;
    regs.V[x] = regs.DT;
}

//Fx0A - LD Vx, K
void LD_F0A(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Stop execution until a key is pressed, or in this case don't move on to the next instruction
    //until a key is pressed, then store the key pressed in Vx.
    uint8_t x = operands >> 8;

    //Maps the keys from a linear key array into the weird keypad chip-8 uses
    if(keys[0])  { regs.V[x] = 0x1; return; }
    if(keys[1])  { regs.V[x] = 0x2; return; }
    if(keys[2])  { regs.V[x] = 0x3; return; }
    if(keys[3])  { regs.V[x] = 0xc; return; }
    if(keys[4])  { regs.V[x] = 0x4; return; }
    if(keys[5])  { regs.V[x] = 0x5; return; }
    if(keys[6])  { regs.V[x] = 0x6; return; }
    if(keys[7])  { regs.V[x] = 0xd; return; }
    if(keys[8])  { regs.V[x] = 0x7; return; }
    if(keys[9])  { regs.V[x] = 0x8; return; }
    if(keys[10]) { regs.V[x] = 0x9; return; }
    if(keys[11]) { regs.V[x] = 0xe; return; }
    if(keys[12]) { regs.V[x] = 0xa; return; }
    if(keys[13]) { regs.V[x] = 0x0; return; }
    if(keys[14]) { regs.V[x] = 0xb; return; }
    if(keys[15]) { regs.V[x] = 0xf; return; }

    regs.PC -= 2;
}

//Fx15 - LD DT, Vx
void LD_F15(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Set DT equal to the value in Vx
    uint8_t x = operands >> 8;
    regs.DT = regs.V[x];
}

//Fx18 - LD ST, Vx
void LD_F18(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Set the Sound Timer to the value in Vx
    uint8_t x = operands >> 8;
    regs.ST = regs.V[x];
}

//Fx1E - ADD I, Vx
void ADD_F(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Add I and the value in Vx, then store the result in I
    uint8_t x = operands >> 8;
    regs.I += regs.V[x];
}

//Fx29 - LD F, Vx
void LD_F29(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Set I equal to the location of the hexadecimal digit corrosponding to the value in Vx
    uint8_t x = operands >> 8;

    //The font data starts at address 0x000 and each sprite is 5 bytes long.
    //So the address I is set to should be Vx * 5
    regs.I = regs.V[x] * 5;
}

//Fx33 - LD B, Vx
void LD_F33(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Put the Binary Coded Decimal(BCD) form of the value in Vx into memory starting at I.
    //The hundreds place is put at I, the tens at I + 1, and the ones at I + 2.
    uint8_t x = operands >> 8;
    uint8_t hundreds = regs.V[x] / 100;
    uint8_t tens = regs.V[x] / 10 - hundreds * 10;
    uint8_t ones = regs.V[x] - (tens * 10 + hundreds * 100);
    mem[regs.I] = hundreds;
    mem[regs.I + 1] = tens;
    mem[regs.I + 2] = ones;
}

//Fx55 - LD [I], Vx
void LD_F55(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Copy registers V0 through Vx into memory at the address stored in I
    uint8_t x = operands >> 8;

    memcpy(mem + regs.I, regs.V, x + 1);
}

//Fx65 - LD Vx, [I]
void LD_F65(uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    //Read registers V0 through Vx starting from the address stored in I
    uint8_t x = operands >> 8;

    memcpy(regs.V, mem + regs.I, x + 1);
}

}