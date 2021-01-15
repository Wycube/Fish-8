#pragma once

#include "Chip8.hpp"

namespace fish {

class Debugger {
private:

    Chip8 *m_instance;
    Interpreter m_interpreter;

public:

    Debugger() { m_instance = nullptr; }
    ~Debugger() { }

    bool attach(Chip8 &emu);
    bool detach();

    bool hasInstance() const;

    uint8_t*  getMemory();
    uint16_t* getStack();
    uint8_t*  getScreen();

    uint8_t*  getVRegister(size_t index);
    uint8_t*  getStackPointer();
    uint16_t* getIRegister();
    uint16_t* getProgramCounter();
    uint8_t*  getDelayTimer();
    uint8_t*  getSoundTimer();

    uint16_t getInstructionAt(uint16_t address);

    std::string disassemble(uint16_t instruction);
};

}