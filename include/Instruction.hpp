#pragma once

#include <unordered_map>

//The instruction is stripped of the operand
//which are replaced by zero.
//Instructions from Cowgod's Chip-8 Reference
static std::unordered_map<uint16_t, std::string> mnemonic = {
    {0x0000, "SYS addr"},           //0nnn
    {0x00e0, "CLS"},                //00E0
    {0x00ee, "RET"},                //00EE
    {0x1000, "JP addr"},            //1nnn
    {0x2000, "CALL addr"},          //2nnn
    {0x3000, "SE Vx, byte"},        //3xkk
    {0x4000, "SNE Vx, byte"},       //4xkk
    {0x5000, "SE Vx, Vy"},          //5xy0
    {0x6000, "LD Vx, byte"},        //6xkk
    {0x7000, "ADD Vx, byte"},       //7xkk
    {0x8000, "LD Vx, Vy"},          //8xy0
    {0x8001, "OR Vx, Vy"},          //8xy1
    {0x8002, "AND Vx, Vy"},         //8xy2
    {0x8003, "XOR Vx, Vy"},         //8xy3
    {0x8004, "ADD Vx, Vy"},         //8xy4
    {0x8005, "SUB Vx, Vy"},         //8xy5
    {0x8006, "SHR Vx {, Vy}"},      //8xy6
    {0x8007, "SUBN Vx, Vy"},        //8xy7
    {0x800e, "SHL Vx, Vy"},         //8xyE
    {0x9000, "SNE Vx, Vy"},         //9xy0
    {0xa000, "LD I, addr"},         //Annn
    {0xb000, "JP V0, addr"},        //Bnnn
    {0xc000, "RND Vx, byte"},       //Cxkk
    {0xd000, "DRW Vx, Vy, nibble"}, //Dxyn
    {0xe09e, "SKP Vx"},             //Ex9E
    {0xe0a1, "SKNP Vx"},            //ExA1
    {0xf007, "LD Vx, DT"},          //Fx07
    {0xf00a, "LD Vx, K"},           //Fx0A
    {0xf015, "LD DT, Vx"},          //Fx15
    {0xf018, "LD ST, Vx"},          //Fx18
    {0xf01e, "ADD I, Vx"},          //Fx1E
    {0xf029, "LD F, Vx"},           //Fx29
    {0xf033, "LD B, Vx"},           //Fx33
    {0xf055, "LD [I], Vx"},         //Fx55
    {0xf065, "LD Vx, [I]"}          //Fx65
};

//Figures out the instruction short to put into
//the mnemic table in order to get the mnemonic
inline uint16_t parseInstruction(uint16_t instr) {
    uint8_t high = instr >> 8;
    uint8_t low = instr & 0x00ff;
    uint8_t opcode = high >> 4;
    uint8_t last = low & 0x000f; //The last nibble

    switch(opcode) {
        case 0x0 : return 0x0000 | low;
        break;

        case 0x1 : return 0x1000;
        break;

        case 0x2 : return 0x2000;
        break;

        case 0x3 : return 0x3000;
        break;

        case 0x4 : return 0x4000;
        break;

        case 0x5 : return 0x5000;
        break;

        case 0x6 : return 0x6000;
        break;

        case 0x7 : return 0x7000;
        break;

        case 0x8 : return 0x8000 | last;
        break;

        case 0x9 : return 0x9000;
        break;

        case 0xa : return 0xa000;
        break;

        case 0xb : return 0xb000;
        break;

        case 0xc : return 0xc000;
        break;

        case 0xd : return 0xd000;
        break;

        case 0xe : return 0xe000 | low;
        break;

        case 0xf : return 0xf000 | low;
        break;
    }

    return 0x0000;
}

namespace fish {

struct Registers;

//Instruction Functions:
//Instructions with the same mnemonic are differentiated by the
//numbers or letters following the underscore. These numbers, or letters
//correspond to the instructions starting digit, or more for the cases of
//AND and LD.
void NOP    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void CLS    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void RET    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void JP_1   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void CALL   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SE_3   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SNE_4  (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SE_5   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_6   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void ADD_7  (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_8   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void OR     (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void AND    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void XOR    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void ADD_8  (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SUB    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SHR    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SUBN   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SHL    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SNE_9  (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_A   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void JP_B   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void RND    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void DRW    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SKP    (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void SKNP   (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_F07 (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_F0A (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_F15 (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_F18 (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void ADD_F  (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_F29 (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_F33 (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_F55 (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);
void LD_F65 (uint16_t operands, Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys);

}