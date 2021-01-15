#include "Application.hpp"

#include <tinyfiledialogs.h>
#define MA_NO_DECODING
#define MA_NO_ENCODING
#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "Log.hpp"

Application::Application() { }

Application::~Application() {   
    cleanup();
}

bool Application::init(int width, int height, const std::string &title, int argc, char *argv[]) {
    //Parse command line arguments
    parseArgs(argc, argv);

    //Check if the window was initialized and set callbacks
    m_window.init(width, height, title);
    if(!m_window.isGood()) { return false; LOG_ERROR("[APP]: Window Failed to Initialize!"); }
    glfwSetWindowUserPointer(m_window.getWindow(), this);
    glfwSetWindowSizeCallback(m_window.getWindow(), resizeCallback);
    m_settings.refresh_screen = refreshWindow;
    glfwSetKeyCallback(m_window.getWindow(), keyCallback);

    //Initalize OpenGL Loader and objects, return false if failed
    if(!initOpengl()) { return false; LOG_ERROR("[APP]: OpenGL Failed to Initialize!"); }

    if(!initAudio()) { return false; LOG_ERROR("[APP]: Audio Failed to Initialize!"); }

    //Initialize ImGui
    if(!m_gui.init(m_window.getWindow(), m_settings)) { return false; LOG_ERROR("[APP]: ImGui Failed to Initialize!"); }

    //Attach the emulator to the Debugger
    m_debug.attach(m_emu);
    m_settings.new_rom_callback = newRomCallback;

    //Load ROM if one was specified
    if(!m_rom_path.empty()) { 
        if(m_settings.new_rom_callback(m_window.getWindow(), m_rom_path.c_str(), m_emu)) {
            m_settings.run_chip8 = true;
            m_settings.status = "Running";
        }
    }

    return true;
}

bool Application::initOpengl() {
    //Initialize the Glad Loader
    if(!gladLoadGL()) {
        LOG_ERROR("[APP]: Glad Failed to Load!");
        return false;
    } else {
        LOG_DEBUG("[APP]: Loaded OpenGl Version %d.%d", GLVersion.major, GLVersion.minor);
    }

    //Create and Bind VBO
    float vertices[] = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f,  1.0f,
         1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    //Create Texture and setup Texture stuff
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    //Nearest Neighbor scaling, so the pixels are sharp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    //Create and Bind Shaders
    const char *vertex_shader = "#version 150\nuniform float dist; uniform vec2 ratio; in vec2 pos; out vec2 texCoord; void main() { "
                                "texCoord = (pos * vec2(1, -1) + 1) / 2; float new_y = pos.y == 1.0f ? pos.y - dist : pos.y;"
                                "gl_Position = vec4(pos.x * ratio.x, new_y * ratio.y, 0.0, 1.0); }";
    const char *fragment_shader = "#version 150\nout vec4 outColor; in vec2 texCoord; uniform sampler2D tex; void main() { outColor = texture(tex, texCoord); }";

    //Vertex Shader
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_shader, nullptr);
    glCompileShader(vertex);

    //Check Compile Results
    GLint compile = GL_FALSE;
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &compile);

    if(compile != GL_TRUE) {
        LOGD_ERROR("[APP]: Vertex Shader Failed to Compile!");
        char log[80];
        glGetShaderInfoLog(vertex, 80, nullptr, log);
        LOG_DEBUG("[APP]: Info Log: %s", log);
        return false;
    }

    //Fragment Shader
    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_shader, nullptr);
    glCompileShader(fragment);

    //Check Compile Results
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &compile);

    if(compile != GL_TRUE) {
        LOGD_ERROR("[APP]: Fragment Shader Failed to Compile!");
        char log[80];
        glGetShaderInfoLog(fragment, 80, nullptr, log);
        LOG_DEBUG("[APP]: Info Log: %s", log);
        return false;
    }

    //Create Program and Attach Shaders
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glUseProgram(program);

    //Some stuff about setting the attribute pos in the shader
    GLint pos = glGetAttribLocation(program, "pos");
    glVertexAttribPointer(pos, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(pos);

    //Get Uniform Locations
    m_uniform_dist = glGetUniformLocation(program, "dist");
    m_uniform_ratio = glGetUniformLocation(program, "ratio");

    //Set clear color to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    return true;
}

bool Application::initAudio() {
    //Configure and initialize the sine wave, frequency 0 so the sine wave won't play when the program starts
    ma_waveform_config config = ma_waveform_config_init(ma_format_f32, 2, 48000, ma_waveform_type_sine, 1.0, 0.0);
    ma_waveform_init(&config, &m_sine_wave);

    //Configure and initialize the device
    ma_device_config device_config;
    device_config = ma_device_config_init(ma_device_type_playback);
    device_config.playback.format   = ma_format_f32;
    device_config.playback.channels = 2;
    device_config.sampleRate        = 48000;
    device_config.dataCallback      = audioCallback;
    device_config.pUserData         = &m_sine_wave;

    if(ma_device_init(nullptr, &device_config, &m_device) != MA_SUCCESS) {
        return false;
    }

    return true;
}

void Application::parseArgs(int argc, char **argv) {
    for(int i = 1; i < argc; i++) {
        if(argv[i][0] == '-') {

            if(strcmp(argv[i], "-d") == 0) {
                m_settings.use_debug = true;
            } else if(strcmp(argv[i], "-i") == 0) {
                m_settings.use_imgui_ini = true;
            } else if((strcmp(argv[i], "-h") == 0) || (strcmp(argv[i], "--help") == 0)) {
                fmt::printf("Usage: %s [options...] rom-path\n\nOptions:\n %-10s - Enables debug GUI and settings\n %-10s - Enables writing of imgui.ini\n %-10s - Shows this help message\n", argv[0], "-d", "-i", "-h --help");
                std::exit(0);
            } else {
                LOG_WARN("Ignoring unknown option %s", argv[i]);
            }  
        } else {
            if(m_rom_path.empty()) {
                m_rom_path = argv[i];
            } else {
                LOG_WARN("Too many files supplied! Only using the first one");
            }
        }
    }
}

void Application::loop() {
    //Call the resize callback once so certain graphics values can be set
    resizeCallback(m_window.getWindow(), m_window.getInitWidth(), m_window.getInitHeight());

    //Used for timing the emulator cycles
    double this_time = glfwGetTime();
    double last_time = this_time;
    double delta = 0, error = 0;

    //Start audio device
    ma_device_start(&m_device);

    //Close Application when the window is closed
    while(!m_window.requestClose()) {
        //Poll glfw for events and check for key presses
        glfwPollEvents();
        updateKeys();

        //Timing
        this_time = glfwGetTime();
        delta = this_time - last_time;
        last_time = this_time;

        //Update emulator and screen texture
        updateEmulator(delta, error);
        updateTexture();

        //Run audio if required
        if(m_emu.shouldPlaySound()) {
            ma_waveform_set_frequency(&m_sine_wave, m_settings.audio_freq);
        } else {
            ma_waveform_set_frequency(&m_sine_wave, 0.0);
        }

        //Start a new frame for ImGui
        if(m_settings.show_gui) {
            m_gui.newFrame();
        }

        //Clear screen and render the chip8's screen buffer
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        //Update ImGui and then Render
        if(m_settings.show_gui) {
            if(m_settings.use_debug) {
                m_gui.updateWithDebug(m_settings, m_emu, m_emu_keys, m_window.getWindow(), m_debug);
            } else {
                m_gui.update(m_settings, m_emu, m_emu_keys, m_window.getWindow());
            }

            m_gui.render();
        }

        //Swap buffers to show the frame
        m_window.swapBuffers();
    }
}

void Application::cleanup() {
    //Detach the emulator from the debugger
    m_debug.detach();

    //Uninit the audio device
    ma_device_uninit(&m_device);

    //Shut down ImGui
    m_gui.shutdown();

    //Destroy the window and terminate glfw
    m_window.destroy();
}

void Application::updateEmulator(double delta, double error) {
    if(m_settings.run_chip8) {
        double num_cycles = m_settings.run_speed * delta; //cycles / sec * sec / frame = cycles / frame
        
        //Accumulate and check error
        error += num_cycles - std::floor(num_cycles);
        if(error >= 1.0) { num_cycles += 1.0; error = error - std::floor(error); }

        //Check for loop
        if(m_settings.detect_loop && m_emu.detectLoop()) {
            m_settings.run_chip8 = false;
            m_settings.status = "Halted (loop detected)";
        }

        //Cycle Emulator
        m_emu.cycle(static_cast<uint32_t>(num_cycles), m_emu_keys, m_settings.stop_timers && !m_running_last);

        m_running_last = true;
    } else {
        m_running_last = false;
    }
}

void Application::updateTexture() {
    uint32_t texture[fish::CHIP8_SCREEN_PIXELS];
    
    //If the pixel is zero set it to the off color, if not it is set to the on color
    for(uint32_t y = 0; y < fish::CHIP8_SCREEN_HEIGHT; y++) {
        for(uint32_t x = 0; x < fish::CHIP8_SCREEN_WIDTH; x++) {
            texture[x + y * fish::CHIP8_SCREEN_WIDTH] = m_emu.getScreenPixel(x, y) > 0 ? floatsToUint(m_settings.foreground) : floatsToUint(m_settings.background);
        }
    }

    //Set the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fish::CHIP8_SCREEN_WIDTH, fish::CHIP8_SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, texture);
}

void Application::updateKeys() {
    //Iterate through key map and query glfw for key presses
    for(uint32_t i = 0; i < fish::CHIP8_NUM_KEYS; i++) {
        m_emu_keys[i] = glfwGetKey(m_window.getWindow(), m_settings.key_map[i]) == GLFW_PRESS;
    }
}

void Application::updateUniforms(int width, int height) {
    float fwidth = static_cast<float>(width);
    float fheight = static_cast<float>(height);

    //Dist is the height of the ImGui main menu bar
    float dist = m_gui.getFrameHeight(m_settings);
    glUniform1f(m_uniform_dist, (dist / fheight) * 2.0f);

    //Ratio is the aspect ratio of the quad the Chip8 screen is on within the window
    if(m_settings.fill_screen) {
        //This just fills up the space in the window
        glUniform2f(m_uniform_ratio, 1.0f, 1.0f);
    } else {
        //This keeps the pixels square
        Vec2f ratio = calcScreenRatio(fwidth, fheight - dist);
        glUniform2f(m_uniform_ratio, ratio.x, ratio.y);
    }
}

Vec2f Application::calcScreenRatio(float width, float height) {
    //Calculate aspect ratio
    float aspect = width / height;
    Vec2f newRatio;

    //If the width is greater or the ratio is equal, base new ratio off of the height, and vice versa
    if(aspect >= 2.0f) {
        newRatio.y = 1.0f;
        newRatio.x = (height * 2.0f) / width;
    } else if(aspect < 2.0f) {
        newRatio.x = 1.0f;
        newRatio.y = (width / 2.0f) / height;
    }

    return newRatio;
}

void Application::resizeCallback(GLFWwindow *window, int width, int height) {
    //Retrieve the instance of Application
    Application *app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

    //Run a partial update loop to update gui and chip8 screen quad
    if(app->m_settings.show_gui) {
        app->m_gui.newFrame();
    }

    //A new ImGui frame needs to be started in order to get an accurate frame height
    app->updateUniforms(width, height);
    
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if(app->m_settings.show_gui) {
        if(app->m_settings.use_debug) {
            app->m_gui.updateWithDebug(app->m_settings, app->m_emu, app->m_emu_keys, window, app->m_debug);
        } else {
            app->m_gui.update(app->m_settings, app->m_emu, app->m_emu_keys, window);
        }

        app->m_gui.render();
    }

    glfwSwapBuffers(window);
}

bool Application::newRomCallback(GLFWwindow *window, const char *path, fish::Chip8 &emu) {
    //Try to load rom and if it fails notify the user with a message box
    if(emu.loadRom(path) != fish::OK) {
        std::string name = base_name(path);
        std::string message = "Failed to load ROM " + name;
        tinyfd_messageBox("Failed to Load New ROM", message.c_str(), "warning", "warning", true);
        LOG_WARN("[APP]: Failed to Load New ROM: %s", name);
        return false;
    }

    //Change title to show the rom
    Application *app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->m_window.setTitle(app->m_title + " - " + emu.getRomInfo().name + emu.getRomInfo().ext);

    return true;
}

//Used for the shortcut keys
void Application::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Application *app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));

    //Shortcuts defined in the gui, Emulator->Control->Start or Stop or Step
    if(key == GLFW_KEY_F1 && action == GLFW_PRESS) { app->m_settings.run_chip8 = true; app->m_settings.status = "Running"; };
    if(key == GLFW_KEY_F2 && action == GLFW_PRESS) { app->m_settings.run_chip8 = false; app->m_settings.status = "Halted (by user)"; };
    if((key == GLFW_KEY_F3 && action == GLFW_PRESS) && !app->m_settings.run_chip8) { app->m_emu.cycle(1, app->m_emu_keys); app->m_settings.status = "Stepped"; };

    //Toggle Gui and call the resize callback so things are resized for when the gui is there or not
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { 
        app->m_settings.show_gui = !app->m_settings.show_gui; 
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        resizeCallback(window, width, height); 
    }
}

//A sort of scaled down version of the resize screen callback, without gui
void Application::refreshWindow(GLFWwindow *window) {
    Application *app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));    
    int width, height;

    glfwGetWindowSize(window, &width, &height);
    app->updateUniforms(width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    glfwSwapBuffers(window);
}

void Application::audioCallback(ma_device *device, void *output, const void *input, ma_uint32 frame_count) {
    ma_waveform *sine_wave;

    MA_ASSERT(device->playback.channels == 2);

    //Get the sine wave data
    sine_wave = reinterpret_cast<ma_waveform*>(device->pUserData);
    MA_ASSERT(sine_wave != nullptr);

    //Output the sine wave data
    ma_waveform_read_pcm_frames(sine_wave, output, frame_count);
}