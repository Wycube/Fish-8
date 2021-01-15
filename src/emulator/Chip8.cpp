#include "Chip8.hpp"

#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#include "Log.hpp"

namespace fish {

const uint8_t FONT_DATA[80] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xf0, 0x10, 0xf0, 0x80, 0xf0, //2
    0xf0, 0x10, 0xf0, 0x10, 0xf0, //3
    0x90, 0x90, 0xf0, 0x10, 0x10, //4
    0xf0, 0x80, 0xf0, 0x10, 0xf0, //5
    0xf0, 0x80, 0xf0, 0x90, 0xf0, //6
    0xf0, 0x10, 0x20, 0x40, 0x40, //7
    0xf0, 0x90, 0xf0, 0x90, 0xf0, //8
    0xf0, 0x90, 0xf0, 0x10, 0xf0, //9
    0xf0, 0x90, 0xf0, 0x90, 0x90, //A
    0xe0, 0x90, 0xe0, 0x90, 0xe0, //B
    0xf0, 0x80, 0x80, 0x80, 0xf0, //C
    0xe0, 0x90, 0x90, 0x90, 0xe0, //D
    0xf0, 0x80, 0xf0, 0x80, 0xf0, //E
    0xf0, 0x80, 0xf0, 0x80, 0x80  //F
};

Chip8::Chip8() {
    init();
}

Chip8::~Chip8() { }

void Chip8::init() {
    //Clear Memory to zeros
    memset(m_mem, 0, CHIP8_MEM_SIZE);

    //Put font data into memory
    memcpy(m_mem, FONT_DATA, 80);

    //Clear registers
    memset(m_regs.V, 0, CHIP8_V_REG_COUNT);
    m_regs.I = 0;
    m_regs.PC = 0x200;
    m_regs.SP = 0;
    m_regs.ST = 0;
    m_regs.DT = 0;
    m_last_pc = 0x200;

    //Clear stack
    memset(m_stack, 0, CHIP8_STACK_MAX * sizeof(uint16_t));

    //Clear screen buffer
    memset(m_screen, 0, CHIP8_SCREEN_PIXELS);

    //Clear Current ROM Info
    m_current_rom  = {};
}

StatusCode Chip8::loadRom(const std::string &path) {
    //Make sure file exists
    if(!std::filesystem::exists(path)) {
        LOG_WARN("[EMU]: ROM %s was not found", base_name(path));
        return FILE_NOT_FOUND;
    }

    size_t size = std::filesystem::file_size(path);

    //Make sure size isn't greater than the chip-8 RAM space for it
    if(size > CHIP8_ROM_MAX || size == 0) {
        LOG_WARN("[EMU]: ROM %s is an invalid size, < 0 or > 0x%X", base_name(path), CHIP8_ROM_MAX);
        return INVALID_FILE_SIZE;
    }

    std::ifstream fstream(path, std::ios::binary);

    //Got to make sure
    if(!fstream.good()) {
        LOG_WARN("[EMU]: ROM %s could not be loaded because filestream is not good", base_name(path));
        return FILE_NOT_GOOD;
    }

    //Reset memory
    init();

    //Fill in rom data for the current loaded ROM
    m_current_rom.size = size;
    m_current_rom.path = path;
    m_current_rom.name = file_name(base_name(path));
    m_current_rom.ext  = path.substr(path.find_last_of('.'));

    //Load rom into a byte array
    uint8_t *rom = new uint8_t[size];

    //Read in the goods
    fstream.read((char*)rom, size);

    //Load ROM into memory, after the reserved space going up to 0x01ff
    memcpy(&m_mem[0x200], rom, size);

    //Delete array
    delete[] rom;

    return OK;
}

void Chip8::cycle(uint32_t num, const bool keys[CHIP8_NUM_KEYS], bool freeze_timers) {
    for(uint32_t i = 0; i < num; i++) {    
        //Update Timers by the time passed whenever the emulator was last updated
        long long this_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
        if(!freeze_timers) {
            m_regs.DT = (this_time - m_last_time) * 0.06 < m_regs.DT ? static_cast<uint8_t>(m_regs.DT - (this_time - m_last_time) * 0.06) : 0;
            m_regs.ST = (this_time - m_last_time) * 0.06 < m_regs.ST ? static_cast<uint8_t>(m_regs.ST - (this_time - m_last_time) * 0.06) : 0;
        }
        m_last_time = this_time;

        m_last_pc = m_regs.PC;
        uint16_t instruction = (m_mem[m_regs.PC] << 8) | m_mem[m_regs.PC + 1];
        m_interpreter.decode(instruction);
        m_interpreter.execute(m_regs, m_mem, m_screen, m_stack, keys);
        m_regs.PC += 2; //Instructions are 2 bytes long
    }
}

uint8_t Chip8::getScreenPixel(uint8_t x, uint8_t y) const {
    return m_screen[x + y * CHIP8_SCREEN_WIDTH];
}

bool Chip8::shouldPlaySound() {
    return m_regs.ST > 0;
}

bool Chip8::detectLoop() {
    //Check if the current instruction is just jumping to itself
    return (m_interpreter.m_opcode == 3) && (m_interpreter.m_operands == m_last_pc);
}

RomInfo Chip8::getRomInfo() const {
    return m_current_rom;
}

}