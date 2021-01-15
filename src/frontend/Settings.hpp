#pragma once

#include "FishCommon.hpp"

//The version of the program
#define FISH8_VERSION_MAJOR 0
#define FISH8_VERSION_MINOR 5
#define FISH8_VERSION_PATCH 0

struct GLFWwindow;

struct Settings {
    bool show_gui       = true;
    bool use_imgui_ini  = false;
    bool use_debug      = false;
    bool run_chip8      = false;
    bool detect_loop    = true;
    std::string status  = "Halted";
    uint32_t run_speed  = 500; //In Hz
    bool stop_timers    = true; //Stop timers while not executing
    bool fill_screen    = false;
    bool gui_overlay    = false;
    bool dis_follow_pc  = false;
    float audio_freq    = 440.0f; //In Hz
    float background[3] = {0.0f, 0.0f, 0.0f}; //Black
    float foreground[3] = {1.0f, 1.0f, 1.0f}; //White
    //The default keys are as follows 1 2 3 4 Q W E R A S D F Z X C V, for 0x0 - 0xf as defined by glfw
    uint32_t key_map[fish::CHIP8_NUM_KEYS]  = {49, 50, 51, 52, 81, 87, 69, 82, 65, 83, 68, 70, 90, 88, 67, 86}; //This determines how keyboard keys map to the chip8's keys
    bool (*new_rom_callback)(GLFWwindow *window, const char *path, fish::Chip8 &emu);
    void (*refresh_screen)(GLFWwindow *window); //Used for instantly updating some value that won't be effected until screen resize
};

//Color Helper Function
static uint32_t floatsToUint(float comps[3]) {
    return static_cast<uint8_t>(comps[0] * 255) << 24 | static_cast<uint8_t>(comps[1] * 255) << 16 | static_cast<uint8_t>(comps[2] * 255) << 8 | 0xff;
}