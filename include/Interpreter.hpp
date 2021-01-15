#pragma once

#include <cstdint>
#include <unordered_map>
#include <array>

namespace fish {

struct Registers;

using InstructionFunc = void(*)(uint16_t, Registers&, uint8_t* /* mem */, uint8_t* /* screen */, uint16_t* /* stack */, const bool* /* keys */);

class Interpreter {
private:
public:
    uint8_t m_opcode;                                //A number corrosponding to the instruction function in the instructions map
    uint16_t m_operands;                             //16-bit in order to hold the possible 12-bit operand

    std::array<InstructionFunc, 35> m_instructions;  //An array containing the instruction's function pointers

//public:

    Interpreter();
    ~Interpreter();

    void decode(uint16_t instr);
    void execute(Registers &regs, uint8_t *mem, uint8_t *screen, uint16_t *stack, const bool *keys); //Executes last instruction decoded
};

}