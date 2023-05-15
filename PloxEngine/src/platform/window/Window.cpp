//
// Created by Ploxie on 2023-05-09.
//
#include "Window.h"
#include "core/Engine.h"
#include <string>

Window::Window(WindowHandle handle, void* rawHandle, const char* title, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
    : m_handle(handle), m_rawHandle(rawHandle), m_positionX(x), m_positionY(y), m_windowWidth(width), m_windowHeight(height), m_cursor(MouseCursor::ARROW), m_mode(WindowMode::WINDOWED)
{
    strcpy_s(m_title, title);
}

const char* Window::GetTitle()
{
    return m_title;
}

WindowHandle Window::GetHandle()
{
    return m_handle;
}
void* Window::GetRawHandle()
{
    return m_rawHandle;
}

Keyboard& Window::GetKeyboard()
{
    return m_keyboard;
}
Mouse& Window::GetMouse()
{
    return m_mouse;
}

void Window::SetFocused(bool focused)
{
    m_isFocused = focused;
}
bool Window::IsFocused() const
{
    return m_isFocused;
}

void Window::SetMouseCursor(MouseCursor cursor)
{
    m_cursor = cursor;
}

Window::MouseCursor Window::GetMouseCursor()
{
    return m_cursor;
}

void Window::SetWindowMode(WindowMode mode)
{
    m_mode = mode;
    Engine::GetRenderer().ActivateFullscreen(this);
}

WindowMode Window::GetWindowMode()
{
    return m_mode;
}

unsigned int Window::GetPositionX() const
{
    return m_positionX;
}

unsigned int Window::GetPositionY() const
{
    return m_positionY;
}

unsigned int Window::GetWidth() const
{
    return m_windowWidth;
}

unsigned int Window::GetHeight() const
{
    return m_windowHeight;
}