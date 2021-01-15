#pragma once

#include <cstdint>
#include <string>

namespace fish {

//Forward declare all classes and structs for includes
class Chip8;
class Debugger;
struct RomInfo;
struct Registers;

enum StatusCode {
    OK, FILE_NOT_FOUND, FILE_NOT_GOOD, INVALID_FILE_SIZE
};

static constexpr uint32_t CHIP8_MEM_SIZE      = 4096; //0x1000 or 4kb
static constexpr uint32_t CHIP8_ROM_MAX       = 3584; //0x0E00
static constexpr uint32_t CHIP8_V_REG_COUNT   = 16;
static constexpr uint32_t CHIP8_STACK_MAX     = 16;
static constexpr uint32_t CHIP8_NUM_KEYS      = 16;
static constexpr uint32_t CHIP8_SCREEN_WIDTH  = 64;
static constexpr uint32_t CHIP8_SCREEN_HEIGHT = 32;
static constexpr uint32_t CHIP8_SCREEN_PIXELS = CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT;

}

//File Name Helper Functions
inline std::string base_name(std::string const &path, std::string const &delims = "/\\") {
  return path.substr(path.find_last_of(delims) + 1);
}

inline std::string file_name(std::string const &name) {
  return name.substr(0, name.find_last_of('.'));
}