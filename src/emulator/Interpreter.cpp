#include "Interpreter.hpp"

#include "Instruction.hpp"
#include "Chip8.hpp"

namespace fish {

Interpreter::Interpreter() {
    m_opcode = 0;
    m_operands = 0;
    m_instructions = {
        NOP,    CLS,    RET,    JP_1,
        CALL,   SE_3,   SNE_4,  SE_5, 
        LD_6,   ADD_7,  LD_8,   OR, 
        AND,    XOR,    ADD_8,  SUB, 
        SHR,    SUBN,   SHL,    SNE_9, 
        LD_A,   JP_B,   RND,    DRW, 
        SKP,    SKNP,   LD_F07, LD_F0A, 
        LD_F15, LD_F18, ADD_F,  LD_F29, 
        LD_F33, LD_F55, LD_F65
    };
}

Interpreter::~Interpreter() { }

void Interpreter::decode(uint16_t instr) {
    uint8_t firstn = instr >> 12; //The first nibble of the instruction
    uint8_t lastn = instr & 0xf;  //The last nibble of the instruction
    uint8_t lastb = instr &0xff;  //The last byte of the instruction
    bool unknown_instr = false;

    switch(firstn) {
        case 0x0 : 
            if(lastb == 0xe0) { m_opcode = 1; }
            else if(lastb == 0xee) { m_opcode = 2; }
            else { unknown_instr = true; }
        break;

        case 0x1 : 
            m_opcode = 3;
        break;

        case 0x2 : 
            m_opcode = 4;
        break;

        case 0x3 : 
            m_opcode = 5;
        break;

        case 0x4 : 
            m_opcode = 6;
        break;

        case 0x5 : 
            if(lastn == 0x0) { m_opcode  = 7; }
            else { unknown_instr = true; }
        break;

        case 0x6 : 
            m_opcode = 8;
        break;

        case 0x7 : 
            m_opcode = 9;
        break;

        case 0x8 : 
            if(lastn == 0x0) { m_opcode = 10; }
            else if(lastn == 0x1) { m_opcode = 11; }
            else if(lastn == 0x2) { m_opcode = 12; }
            else if(lastn == 0x3) { m_opcode = 13; }
            else if(lastn == 0x4) { m_opcode = 14; }
            else if(lastn == 0x5) { m_opcode = 15; }
            else if(lastn == 0x6) { m_opcode = 16; }
            else if(lastn == 0x7) { m_opcode = 17; }
            else if(lastn == 0xe) { m_opcode = 18; }
            else { unknown_instr = true; }
        break;

        case 0x9 : 
            if(lastn == 0x0) { m_opcode = 19; }
            else { unknown_instr = true; }
        break;

        case 0xa : 
            m_opcode = 20;
        break;

        case 0xb : 
            m_opcode = 21;
        break;

        case 0xc : 
            m_opcode = 22;
        break;

        case 0xd : 
            m_opcode = 23;
        break;

        case 0xe : 
            if(lastb == 0x9e) { m_opcode = 24; }
            else if(lastb == 0xa1) { m_opcode = 25; }
            else { unknown_instr = true; }
        break;

        case 0xf : 
            if(lastb == 0x07) { m_opcode = 26; }
            else if(lastb == 0x0a) { m_opcode = 27; }
            else if(lastb == 0x15) { m_opcode = 28; }
            else if(lastb == 0x18) { m_opcode = 29; }
            else if(lastb == 0x1e) { m_opcode = 30; }
            else if(lastb == 0x29) { m_opcode = 31; }
            else if(lastb == 0x33) { m_opcode = 32; }
            else if(lastb == 0x55) { m_opcode = 33; }
            else if(lastb == 0x65) { m_opcode = 34; }
            else { unknown_instr = true; }
        break;
    }

    if(unknown_instr) { 
        m_opcode = 0; 
        m_operands = 0; 
    } else {
         m_operands = instr & 0xfff;
    }
}

void Interpreter::execute(Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys) {
    m_instructions[m_opcode](m_operands, regs, mem, screen, stack, keys);
}

}