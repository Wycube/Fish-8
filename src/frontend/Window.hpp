#pragma once

#include <string>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window {
private:

    GLFWwindow *m_window;
    int m_width, m_init_width;
    int m_height, m_init_height;

    bool m_good;

public:

    Window();
    ~Window();

    void init(int width, int height, const std::string &title);
    void swapBuffers();
    bool requestClose();
    bool isGood();
    void destroy();

    GLFWwindow* getWindow();
    int getWidth();
    int getHeight();
    int getInitWidth();
    int getInitHeight();
    void setTitle(const std::string &title);
};