#pragma once

#include <imgui.h>

#include "Settings.hpp"
#include "Debugger.hpp"

enum Theme {
    DARK, LIGHT
};

class Gui {
private:

    bool m_show_rom_popup = false;

    bool m_show_settings  = false;
    int32_t edit_key      = -1;

    bool m_show_emu_info  = false;
    bool m_show_emu_mem   = false;
    bool m_show_emu_reg   = false;
    bool m_show_emu_dis   = false;

    bool m_show_about     = false;

    Theme m_theme = DARK;

    ImGuiWindowFlags m_overlay_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | 
                                       ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    void changeTheme();
    static bool stackToString(void *data, int index, const char **out_text);

public:

    Gui();
    ~Gui();

    bool init(GLFWwindow *window, Settings &settings);
    void shutdown();

    void newFrame();
    void render();
    void update(Settings &settings, fish::Chip8 &emu, const bool *keys, GLFWwindow *window);
    void updateWithDebug(Settings &settings, fish::Chip8 &emu, const bool *keys, GLFWwindow *window, fish::Debugger &debug);  //This method contains more debug gui

    float getFrameHeight(Settings &settings);  //This returns the height of the Main Menu Bar
};