#include "Window.hpp"

#include <iostream>
#include <iomanip>

#include "res/Icon.hpp"
#include "Log.hpp"

Window::Window() { }

Window::~Window() {
    destroy();
}

void Window::init(int width, int height, const std::string &title) {
    m_good = true;
    m_init_width = width;
    m_init_height = height;
    m_width = width;
    m_height = height;

    //Initialize GLFW
    if(!glfwInit()) {
        m_good = false;
        const char *desc;
        glfwGetError(&desc);
        LOG_ERROR("[WIN]: GLFW Failed to Initialize! Error: %s", desc);
    }

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    if(!m_window) {
        m_good = false;
        const char *desc;
        glfwGetError(&desc);
        LOG_ERROR("[WIN]: GLFW Window Failed to Initialize! Error: %s", desc);
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);

    GLFWimage image = GLFWimage{icon_w, icon_h, icon};
    glfwSetWindowIcon(m_window, 1, &image);
}

void Window::swapBuffers() {
    glfwSwapBuffers(m_window);
}

bool Window::requestClose() {
    return glfwWindowShouldClose(m_window);
}

bool Window::isGood() {
    return m_good;
}

void Window::destroy() {
    //Check if window is not destroyed already
    if(m_window != nullptr) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        m_good = false;
        m_window = nullptr;
    }
}

GLFWwindow* Window::getWindow() {
    return m_window;
}

int Window::getWidth() {
    glfwGetWindowSize(m_window, &m_width, nullptr);
    return m_width;
}

int Window::getHeight() {
    glfwGetWindowSize(m_window, nullptr, &m_height);
    return m_width;
}

int Window::getInitWidth() {
    return m_init_width;
}

int Window::getInitHeight() {
    return m_init_height;
}

void Window::setTitle(const std::string &title) {
    glfwSetWindowTitle(m_window, title.c_str());
}