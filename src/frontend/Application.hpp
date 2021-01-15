#pragma once

#include <glad/glad.h>
#include <miniaudio.h>

#include "Window.hpp"
#undef APIENTRY //Stops a warning
#include "Gui.hpp"
#include "Chip8.hpp"
#include "Debugger.hpp"

struct Vec2f {
    float x;
    float y;
};

class Application {
private:

    Window m_window;
    std::string m_title;
    Gui m_gui;

    Settings m_settings;
    std::string m_rom_path;

    bool m_running_last;
    bool m_emu_keys[fish::CHIP8_NUM_KEYS];
    fish::Chip8 m_emu;
    fish::Debugger m_debug;

    GLint m_uniform_dist;
    GLint m_uniform_ratio;

    ma_device m_device;
    ma_waveform m_sine_wave;

    bool initOpengl();
    bool initAudio();
    void parseArgs(int argc, char **argv);

    void updateEmulator(double delta, double error);
    void updateTexture();
    void updateKeys();
    void updateUniforms(int width, int height);
    static Vec2f calcScreenRatio(float width, float height);
    static bool detectLoop(fish::Chip8 &emu);

    static void resizeCallback(GLFWwindow *window, int width, int height);
    static bool newRomCallback(GLFWwindow *window, const char *path, fish::Chip8 &emu);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void refreshWindow(GLFWwindow *window);
    static void audioCallback(ma_device *device, void *output, const void *input, ma_uint32 frame_count);

public:

    Application();
    ~Application();

    bool init(int width, int height, const std::string &title, int argc, char *argv[]);
    void loop();
    void cleanup();
};