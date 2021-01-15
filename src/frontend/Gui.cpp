#define _CRT_SECURE_NO_WARNINGS
#include "Gui.hpp"

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_memory_editor.h>
#include <imgui_internal.h>
#include <tinyfiledialogs.h>
#include <fmt/printf.h>

#include "res/Inconsolata.hpp"
#include "Chip8.hpp"
#include "Application.hpp"
#include "Log.hpp"

Gui::Gui() { }

Gui::~Gui() {
    shutdown();
}

bool Gui::init(GLFWwindow *window, Settings &settings) {
    bool success = true;

    //Setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO(); (void)io;

    //Add inconsolata font
    io.Fonts->AddFontFromMemoryCompressedTTF(inconsolata_compressed_data, inconsolata_compressed_size, 15);
    io.Fonts->Build();

    if(!settings.use_imgui_ini) {
        io.IniFilename = nullptr;
    }

    success &= ImGui_ImplGlfw_InitForOpenGL(window, true);
    success &= ImGui_ImplOpenGL3_Init("#version 150");

    //Change some stuff
    if(success) {
        ImGuiStyle &style = ImGui::GetStyle();
        style.WindowRounding = 0;
        style.FrameRounding = 0;
        style.ScrollbarRounding = 0;
        style.TabRounding = 0;
        style.WindowBorderSize = 0;
        style.FrameBorderSize = 0;

        changeTheme();
    }

    return success;
}

void Gui::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void Gui::newFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void Gui::render() {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Gui::update(Settings &settings, fish::Chip8 &emu, const bool *keys, GLFWwindow *window) {
    //Main Menu Bar
    ImGui::BeginMainMenuBar();

    if(ImGui::BeginMenu("File")) {
        ImGui::MenuItem("Open", nullptr, &m_show_rom_popup, !settings.run_chip8); //Don't allow new roms to be loaded while running
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Options")) {
        ImGui::MenuItem("Settings", nullptr, &m_show_settings);

        if(ImGui::BeginMenu("Theme")) {
            if(ImGui::MenuItem("Light", nullptr, m_theme == LIGHT)) { m_theme = LIGHT; changeTheme(); }
            if(ImGui::MenuItem("Dark", nullptr, m_theme == DARK)) { m_theme = DARK; changeTheme(); }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Emulator")) {
        ImGui::MenuItem("Info", nullptr, &m_show_emu_info);
        
        if(ImGui::BeginMenu("Control")) {
            if(ImGui::MenuItem("Start", "F1", false, !settings.run_chip8)) { settings.run_chip8 = true; settings.status = "Running"; }
            if(ImGui::MenuItem("Stop", "F2", false, settings.run_chip8)) { settings.run_chip8 = false; settings.status = "Halted (by user)"; }
            if(ImGui::MenuItem("Step", "F3", false, !settings.run_chip8)) { emu.cycle(1, keys); settings.status = "Stepped"; }

            

            ImGui::EndMenu();
        }

        if(settings.use_debug) {
            if(ImGui::BeginMenu("Debug")) {
                ImGui::MenuItem("Memory", nullptr, &m_show_emu_mem);
                ImGui::MenuItem("Registers", nullptr, &m_show_emu_reg);
                ImGui::MenuItem("Disassembly", nullptr, &m_show_emu_dis);
                ImGui::EndMenu();
            }
        }
        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("Help")) {
        ImGui::MenuItem("About", nullptr, &m_show_about);
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();

    
    //Windows
    static const int num_filters = 3;
    static const char * const filters[num_filters] = {"*.rom", "*.c8", "*.ch8"};
    static const uint32_t uint_slider_min = 0, uint_slider_max = 2000;
    static const char *key_names[fish::CHIP8_NUM_KEYS] = {"1:", "2:", "3:", "C:", "4:", "5:", "6:", "D:", "7:", "8:", "9:", "E:", "A:", "0:", "B:", "F:"};

    if(m_show_rom_popup) {
        //Open a native file dialog popup in order to get new rom
        const char *open_file_path = tinyfd_openFileDialog("Open", "./", num_filters, filters, nullptr, 0);

        if(open_file_path != nullptr) {
            if(settings.new_rom_callback(window, open_file_path, emu)) { 
                settings.run_chip8 = true;
                settings.status = "Running";
            }
        }

        m_show_rom_popup = false;
    }

    if(m_show_settings) {
        ImGui::Begin("Settings", &m_show_settings);

        ImGui::Text("Palette:");
        ImGui::ColorEdit3("Foreground Color", settings.foreground);
        ImGui::ColorEdit3("Background Color", settings.background);
        ImGui::Separator();

        ImGui::Text("Execution:");
        ImGui::SliderScalar("Run Speed", ImGuiDataType_U32, &settings.run_speed, &uint_slider_min, &uint_slider_max, "%d Hz", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Checkbox("Freeze Timers When Not Running", &settings.stop_timers);
        ImGui::Checkbox("Halt on Loop Detection", &settings.detect_loop);
        ImGui::Separator();

        ImGui::Text("Key Map: <[Chip8 Key]: [Mapped Key]>");
        
        for(uint32_t i = 0; i < fish::CHIP8_NUM_KEYS; i++) {
            ImGui::Text(key_names[i]); ImGui::SameLine();
            if(ImGui::Button(edit_key == i ? "Press a key" : glfwGetKeyName(settings.key_map[i], 0))) edit_key = i;

            if(!(i % 4 == 3)) {
                ImGui::SameLine();
            }
        }

        //This is a dumb way of doing this but whatever
        bool key_in_map = false;
        if(edit_key != -1 && ImGui::IsWindowFocused()) {
            for(uint32_t key = 0; key < GLFW_KEY_GRAVE_ACCENT; key++) {
                key_in_map = false;
                
                //Check key isn't already in key_map
                for(uint32_t i = 0; i < fish::CHIP8_NUM_KEYS; i++) {
                    if(settings.key_map[i] == key && edit_key != i) {
                        key_in_map = true;
                    }
                }

                //Don't allow spaces, keycode 32, to be set
                if(glfwGetKey(window, key) == GLFW_PRESS && key != 32 && !key_in_map) {
                    settings.key_map[edit_key] = key;
                    edit_key = -1;
                    break;
                }
            }
        }
        ImGui::Separator();
        
        ImGui::Text("Audio:");
        ImGui::SliderFloat("Frequency", &settings.audio_freq, 220.0f, 2000.0f, "%.1f Hz", ImGuiSliderFlags_AlwaysClamp);
        ImGui::Separator();
        
        ImGui::Text("Graphics:");

        if(ImGui::Checkbox("Fill Screen", &settings.fill_screen)) {
            //Show changes
            settings.refresh_screen(window);
        }

        ImGui::Checkbox("Info Overlay", &settings.gui_overlay);

        if(settings.use_debug) {
            ImGui::Separator();
            ImGui::Text("Debug:");

            ImGui::Checkbox("Follow PC in Disassembly", &settings.dis_follow_pc);
        }

        ImGui::End();
    } else {
        //Reset the key being edited if the window is closed
        edit_key = -1;
    }

    if(settings.gui_overlay) {
        ImGui::SetNextWindowPos({5, getFrameHeight(settings) + 5});
        ImGui::SetNextWindowBgAlpha(0.5f);
        ImGui::Begin("Info", &settings.gui_overlay, m_overlay_flags);
        
        ImGui::Text("Framerate: %.1f (vsync)", ImGui::GetIO().Framerate);
        ImGui::Text("Status: %s", settings.status.c_str());

        if(settings.use_debug) {
            ImGui::Separator();
            ImGui::Text("GPU: %s", glGetString(GL_RENDERER));
            ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
            ImGui::Text("OpenGL Version: %s", glGetString(GL_VERSION));
        }
        
        ImGui::End();
    }

    if(m_show_emu_info) {
        ImGui::Begin("Info", &m_show_emu_info, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

        ImGui::Text("Rom Size: %i bytes", emu.getRomInfo().size);
        ImGui::Text("Rom Path: %s", emu.getRomInfo().path.c_str());

        ImGui::End();
    }
 
    if(m_show_about) {
        ImGui::OpenPopup("About");
        ImGuiIO &io = ImGui::GetIO();
        ImGui::SetNextWindowPos({io.DisplaySize.x / 2 - io.DisplaySize.x / 4, io.DisplaySize.y / 2 - io.DisplaySize.y / 4});

        ImGui::SetNextWindowPos({io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f}, ImGuiCond_Always, {0.5f, 0.5f});

        if(ImGui::BeginPopupModal("About", &m_show_about, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Fish-8 version %i.%i.%i, by Spencer Burton", FISH8_VERSION_MAJOR, FISH8_VERSION_MINOR, FISH8_VERSION_PATCH);
            ImGui::Separator();
            ImGui::Text("Acknowledgments:");
            ImGui::Text("GLFW           - Windowing");
            ImGui::Text("Glad           - OpenGL Loader");
            ImGui::Text("Dear ImGui     - GUI");
            ImGui::Text("fmt            - Formatting Text in Logs");
            ImGui::Text("tinyfiledialog - Native File Dialogs");

            ImGui::EndPopup();
        }
    }
}

void Gui::updateWithDebug(Settings &settings, fish::Chip8 &emu, const bool *keys, GLFWwindow *window, fish::Debugger &debug) {
    update(settings, emu, keys, window);

    if(m_show_emu_mem) {
        static MemoryEditor mem_edit;
        m_show_emu_mem = mem_edit.Open;
        mem_edit.DrawWindow("Memory", debug.getMemory(), fish::CHIP8_MEM_SIZE);
    }

    if(m_show_emu_reg) {
        ImGui::Begin("Registers / Stack", &m_show_emu_reg, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize);

        static uint8_t u8_slider_min = 0, u8_slider_max = 255;
        static uint16_t u16_slider_min = 0, u16_slider_max = 0xfff;
        static uint8_t sp_slider_min = 0, sp_slider_max = 16;
        ImGuiSliderFlags slider_flags = ImGuiSliderFlags_AlwaysClamp | ImGuiSliderFlags_NoRoundToFormat;
        //No Changing Registers while running
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled | ImGuiItemFlags_ReadOnly, settings.run_chip8);
        ImGui::PushItemWidth(100.0f);

        //V Registers
        for(uint32_t i = 0; i < fish::CHIP8_V_REG_COUNT; i++) {
            ImGui::Text("V%X:", i); ImGui::SameLine();
            ImGui::SliderScalar(fmt::sprintf("##Reg_V%X", i).c_str(), ImGuiDataType_U8, debug.getVRegister(i), &u8_slider_min, &u8_slider_max, "%02X", slider_flags);

            if(i % 2 == 0) {
                ImGui::SameLine();
            }
        }
        ImGui::PopItemWidth();
        ImGui::Separator();

        //Program Counter and I Register
        ImGui::Text("PC:"); ImGui::SameLine();
        ImGui::SliderScalar("##Reg_PC", ImGuiDataType_U16, debug.getProgramCounter(), &u16_slider_min, &u16_slider_max, "%03X", slider_flags);
        ImGui::Text("I :"); ImGui::SameLine();
        ImGui::SliderScalar("##Reg_I", ImGuiDataType_U16, debug.getIRegister(), &u16_slider_min, &u16_slider_max, "%03X", slider_flags);
        ImGui::Separator();

        //Timers
        ImGui::Text("Timers");
        ImGui::Text("Delay:"); ImGui::SameLine();
        ImGui::SliderScalar("##Reg_DT", ImGuiDataType_U8, debug.getDelayTimer(), &u8_slider_min, &u8_slider_max, "%02X", slider_flags);
        ImGui::Text("Sound:"); ImGui::SameLine();
        ImGui::SliderScalar("##Reg_ST", ImGuiDataType_U8, debug.getSoundTimer(), &u8_slider_min, &u8_slider_max, "%02X", slider_flags);
        ImGui::Separator();

        //Stack Pointer and Stack
        ImGui::Text("Stack");
        ImGui::Text("SP:"); ImGui::SameLine();
        ImGui::SliderScalar("##Reg_SP", ImGuiDataType_U8, debug.getStackPointer(), &sp_slider_min, &sp_slider_max, "%i", slider_flags);

        static const uint16_t stack_inp_step = 1;

        for(uint32_t i = 0; i < fish::CHIP8_STACK_MAX; i++) {
            ImGui::Text("%2i", i); ImGui::SameLine();
            ImGui::InputScalar(fmt::sprintf("##Stack%i", i).c_str(), ImGuiDataType_U16, &(debug.getStack())[i], &stack_inp_step, &stack_inp_step, "%03X");
        }

        ImGui::PopItemFlag();
        ImGui::End();
    }

    if(m_show_emu_dis) {
        ImGui::Begin("Disassembly", &m_show_emu_dis, ImGuiWindowFlags_AlwaysAutoResize);

        for(uint16_t i = 0; i < emu.getRomInfo().size; i += 2) {
            uint16_t instr = debug.getInstructionAt(0x200 + i);
            
            if((0x200 + i) == *debug.getProgramCounter()) {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, 0x880000ff);
                if(settings.run_chip8 && settings.dis_follow_pc) { ImGui::SetScrollHereY(); }
            } else {
                ImGui::PushStyleColor(ImGuiCol_FrameBg, 0x00000000);
            }

            ImGui::BeginChildFrame(1000000 + i, {220, 20}, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs);
            ImGui::Text("%03X : %-14s - %04X", 0x200 + i, debug.disassemble(instr).c_str(), instr);
            ImGui::EndChildFrame();

            ImGui::PopStyleColor();
        }

        ImGui::End();
    }
}

void Gui::changeTheme() {
    if(m_theme == DARK) {
        ImGui::StyleColorsDark();
    } else if(m_theme == LIGHT) {
        ImGui::StyleColorsLight();
    }
}

float Gui::getFrameHeight(Settings &settings) {
    return settings.show_gui ? ImGui::GetFrameHeight() : 0;
}