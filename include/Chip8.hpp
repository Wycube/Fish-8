#pragma once

#include <string>

#include "FishCommon.hpp"
#include "Interpreter.hpp"

namespace fish {

struct RomInfo {
    std::string path = "";
    std::string name = "";
    std::string ext  = "";
    size_t      size = 0;
};

struct Registers {
    union {
        struct {
            uint8_t V0;
            uint8_t V1;
            uint8_t V2;
            uint8_t V3;
            uint8_t V4;
            uint8_t V5;
            uint8_t V6;
            uint8_t V7;
            uint8_t V8;
            uint8_t V9;
            uint8_t VA;
            uint8_t VB;
            uint8_t VC;
            uint8_t VD;
            uint8_t VE;
            uint8_t VF; //Used as a flag, shouldn't be modified by programs
        };
        uint8_t V[CHIP8_V_REG_COUNT];
    }; //16 V registers

    uint16_t I;  //I, address register
    uint16_t PC; //PC, program counter
    uint8_t  SP; //SP, stack pointer
    uint8_t  DT; //DT, delay timer
    uint8_t  ST; //ST, sound timer
};


class Chip8 {
private:

    void init();

    Registers m_regs;                       //All of the registers  
    uint16_t m_last_pc;                     //PC on the last instruction       
    uint16_t m_instr;                       //The current instruction
    uint16_t m_stack[CHIP8_STACK_MAX];      //The stack holds return addresses from subroutines, it can hold up to 16
    uint8_t m_mem[CHIP8_MEM_SIZE];          //The 4kb(4096) memory, 0x0000 - 0x01ff is reserved for interpreter and font data
    uint8_t m_screen[CHIP8_SCREEN_PIXELS];  //The screen buffer, I'm using a byte for each pixel but the screen is monochrome and only has 1bpp

    long long m_last_time;                  //Used for checking the time between checking the timers

    size_t m_rom_size;
    std::string m_rom_path;
    RomInfo m_current_rom;

    Interpreter m_interpreter;

public:

    Chip8();
    ~Chip8();

    StatusCode loadRom(const std::string &path);
    RomInfo getRomInfo() const;

    void cycle(uint32_t num, const bool keys[CHIP8_NUM_KEYS], bool freeze_timers = false);
    uint8_t getScreenPixel(uint8_t x, uint8_t y) const;
    bool shouldPlaySound();
    bool detectLoop();

    friend class Debugger;
};

}