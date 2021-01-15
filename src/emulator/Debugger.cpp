#include "Debugger.hpp"

#include <fmt/printf.h>

namespace fish {

static std::array<const char*, 35> mnemonics = {
    "NOP/SYS", "CLS", "RET", "JP %03X",
    "CALL %03X", "SE V%X, %i", "SNE V%X, %i", "SE V%X, V%X",
    "LD V%X, %i", "ADD V%X, %i", "LD V%X, V%X", "OR V%X, V%X",
    "AND V%X, V%X", "XOR V%X, V%X", "ADD V%X, V%X", "SUB V%X, V%X",
    "SHR V%X", "SUBN V%X, V%X", "SHL V%X", "SNE V%X, V%X",
    "LD I, %03X", "JP V0, %03X", "RND V%X, %02X", "DRW V%X, V%X, %i",
    "SKP V%X", "SKNP V%X", "LD V%X, DT", "LD V%X, Key",
    "LD DT, V%X", "LD ST, V%X", "ADD I, V%X", "LD F, V%X",
    "LD B, V%X", "LD [I], V%X", "LD V%X, [I]"
};

bool Debugger::attach(Chip8 &emu) {
    if(m_instance == nullptr) {
        m_instance = &emu;
    } else {
        return false;
    }

    return true;
}

bool Debugger::detach() {
    if(m_instance != nullptr) {
        m_instance = nullptr;
    } else {
        return false;
    }

    return true;
}

bool Debugger::hasInstance() const {
    return m_instance != nullptr;
}


uint8_t* Debugger::getMemory() {
    return m_instance->m_mem;
}

uint16_t* Debugger::getStack() {
    return m_instance->m_stack;
}

uint8_t* Debugger::getScreen() {
    return m_instance->m_screen;
}


uint8_t* Debugger::getVRegister(size_t index) {
    return &m_instance->m_regs.V[index];
}

uint8_t* Debugger::getStackPointer() {
    return &m_instance->m_regs.SP;
}

uint16_t* Debugger::getIRegister() {
    return &m_instance->m_regs.I;
}

uint16_t* Debugger::getProgramCounter() {
    return &m_instance->m_regs.PC;
}

uint8_t* Debugger::getDelayTimer() {
    return &m_instance->m_regs.DT;
}

uint8_t* Debugger::getSoundTimer() {
    return &m_instance->m_regs.ST;
}

uint16_t Debugger::getInstructionAt(uint16_t address) {
    uint8_t high = m_instance->m_mem[address];
    uint8_t low = m_instance->m_mem[address + 1];

    return (high << 8) | low;
}

std::string Debugger::disassemble(uint16_t instruction) {
    m_interpreter.decode(instruction);
    uint16_t a, b, c; //Parameters

    a = m_interpreter.m_operands >> 8;
    b = m_interpreter.m_operands >> 4 & 0xf;
    c = m_interpreter.m_operands & 0xf;

    switch(m_interpreter.m_opcode) {
        case 3 :
        case 4 : a = m_interpreter.m_operands;
        break;

        case 5 :
        case 6 :
        case 8 :
        case 9 :
        case 22: b = m_interpreter.m_operands & 0xff;
        break;

        case 20:
        case 21: b = m_interpreter.m_operands;
        break;
    }

    return fmt::sprintf(mnemonics[m_interpreter.m_opcode], a, b, c);
}

}