//
// Created by Ploxie on 2023-03-22.
//
#include "Window.h"
#include <cassert>
#define GLFW_EXPOSE_NATIVE_WIN32
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#include "utility/utilities.h"
#include "core/engine.h"

void WindowSizeCallback(GLFWwindow* window, int width, int height);
void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
void CursorPosCallback(GLFWwindow* window, double x, double y);
void CursorEnterCallback(GLFWwindow* window, int entered);
void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void CharCallback(GLFWwindow* window, unsigned int codepoint);

Window::Window(unsigned int width, unsigned int height, Window::WindowMode mode, const char* title)
: m_handle(), m_cursors(), m_mode(mode), m_newMode(mode), m_windowWidth(width), m_windowHeight(height), m_width(width), m_height(height), m_windowedWidth(width), m_windowedHeight(height)
{
    strcpy_s(m_title, title);
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    switch(m_mode)
    {
        case WindowMode::WINDOWED:
        {
            m_handle = glfwCreateWindow(m_width, m_height, m_title, nullptr, nullptr);
            break;
        }
        case WindowMode::FULL_SCREEN:
        {
            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* vidMode = glfwGetVideoMode(monitor);

            m_handle = glfwCreateWindow(vidMode->width, vidMode->height, m_title, monitor, nullptr);
            break;
        }
        default:
        {
            assert(false);
            break;
        }
    }

    if(!m_handle)
    {
        glfwTerminate();
        Util::FatalExit("Failed to create GLFW window", EXIT_FAILURE);
    }

    glfwSetWindowSizeCallback(m_handle, WindowSizeCallback);
    glfwSetFramebufferSizeCallback(m_handle, FrameBufferSizeCallback);
    glfwSetCursorPosCallback(m_handle, CursorPosCallback);
    glfwSetCursorEnterCallback(m_handle, CursorEnterCallback);
    glfwSetScrollCallback(m_handle, ScrollCallback);
    glfwSetMouseButtonCallback(m_handle, MouseButtonCallback);
    glfwSetKeyCallback(m_handle, KeyCallback);
    glfwSetCharCallback(m_handle, CharCallback);

    glfwSetWindowUserPointer(m_handle, this);

    {
        int w = 0;
        int h = 0;

        glfwGetWindowSize(m_handle, &w, &h);
        m_windowWidth = static_cast<uint32_t>(w);
        m_windowHeight = static_cast<uint32_t>(h);

        glfwGetFramebufferSize(m_handle, &w, &h);
        m_width = static_cast<uint32_t>(w);
        m_height = static_cast<uint32_t>(h);
    }

    m_cursors[(size_t)MouseCursor::ARROW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_cursors[(size_t)MouseCursor::TEXT] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    m_cursors[(size_t)MouseCursor::RESIZE_ALL] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_cursors[(size_t)MouseCursor::RESIZE_VERTICAL] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    m_cursors[(size_t)MouseCursor::RESIZE_HORIZONTAL] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    m_cursors[(size_t)MouseCursor::RESIZE_TRBL] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_cursors[(size_t)MouseCursor::RESIZE_TLBR] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    m_cursors[(size_t)MouseCursor::HAND] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
    m_cursors[(size_t)MouseCursor::CROSSHAIR] = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
}

Window::~Window()
{
    size_t cursorCount = sizeof(m_cursors) / sizeof(m_cursors[0]);
    for(size_t i = 0 ; i < cursorCount; i++)
    {
        glfwDestroyCursor(m_cursors[i]);
    }

    glfwDestroyWindow(m_handle);
    glfwTerminate();
}

void Window::PollEvents()
{
    m_configurationChanged = false;
    glfwPollEvents();
    if(m_newMode != m_mode)
    {
        switch(m_newMode)
        {
            case WindowMode::WINDOWED:
            {
                const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                int x = vidMode->width / 2 - m_windowedWidth / 2;
                int y = vidMode->height / 2 - m_windowedHeight / 2;
                y = (y < 32) ? 32 : y;
                glfwSetWindowMonitor(m_handle, nullptr, x, y, m_windowedWidth, m_windowedHeight, GLFW_DONT_CARE);
                break;
            }
            case WindowMode::FULL_SCREEN:
            {
                m_windowedWidth = m_windowWidth;
                m_windowedHeight = m_windowHeight;
                const GLFWvidmode* vidMode = glfwGetVideoMode(glfwGetPrimaryMonitor());
                glfwSetWindowMonitor(m_handle, glfwGetPrimaryMonitor(), 0, 0, vidMode->width, vidMode->height, GLFW_DONT_CARE);
                break;
            }
            default:
            {
                assert(false);
                break;
            }
        }

        m_configurationChanged = true;
        m_mode = m_newMode;
    }
}

void* Window::GetHandle() const
{
    return static_cast<void*>(m_handle);
}

void* Window::GetHandleRaw() const
{
#ifdef _WIN32
    return glfwGetWin32Window(m_handle);
#else
    return nullptr;
#endif
}

unsigned int Window::GetWidth() const
{
    return m_width;
}

unsigned int Window::GetHeight() const
{
    return m_height;
}

unsigned int Window::GetWindowWidth() const
{
    return m_windowWidth;
}
unsigned int Window::GetWindowHeight() const
{
    return m_windowHeight;
}
bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_handle) != 0;
}

void Window::SetMouseCursorMode(MouseCursorMode mode)
{
    switch(mode)
    {
        case MouseCursorMode::NORMAL:
            glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            break;
        case MouseCursorMode::HIDDEN:
            glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            break;
        case MouseCursorMode::DISABLED:
            glfwSetInputMode(m_handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            break;
        default:
            assert(false);
            break;
    }

    m_cursorMode = mode;
}

Window::MouseCursorMode Window::GetMouseCursorMode() const
{
    return m_cursorMode;
}

void Window::SetWindowMode(WindowMode windowMode)
{
    m_newMode = windowMode;
}

Window::WindowMode Window::GetWindowMode() const
{
    return m_mode;
}

void Window::SetTitle(const char* title)
{
    strcpy_s(m_title, title);
    glfwSetWindowTitle(m_handle, m_title);
}

void Window::SetMouseCursor(MouseCursor cursor)
{
    assert((size_t)cursor < (sizeof(m_cursors) / sizeof(m_cursors[0])));
    glfwSetCursor(m_handle, m_cursors[static_cast<size_t>(cursor)]);
}

void Window::SetCursorPosition(float x, float y)
{
    glfwSetCursorPos(m_handle, x, y);
}

bool Window::IsFocused() const
{
    return glfwGetWindowAttrib(m_handle, GLFW_FOCUSED) != 0;
}

void WindowSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* windowPointer = static_cast<Window*>(glfwGetWindowUserPointer(window));
    windowPointer->m_windowWidth = width;
    windowPointer->m_windowHeight = height;
    windowPointer->m_configurationChanged = true;
}

void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
    auto* windowPointer = static_cast<Window*>(glfwGetWindowUserPointer(window));
    windowPointer->m_width = width;
    windowPointer->m_height = height;
    windowPointer->m_configurationChanged = true;
}

void CursorPosCallback(GLFWwindow* window, double x, double y)
{
    Engine::Input->OnMouseMove(x, y);
}

void CursorEnterCallback(GLFWwindow* window, int entered)
{

}

void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    Engine::Input->OnScroll(xOffset, yOffset);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Engine::Input->OnMouseButton(static_cast<MouseButton>(button), static_cast<InputAction>(action));
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Engine::Input->OnKey(static_cast<Key>(key), static_cast<InputAction>(action));
}

void CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    Engine::Input->OnChar(codepoint);
}