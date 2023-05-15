//
// Created by Ploxie on 2023-05-09.
//

#pragma once
#include "input/input.h"

enum WindowHandle : size_t
{
    NULL_WINDOW_HANDLE
};

enum class WindowMode
{
    WINDOWED,
    FULLSCREEN,
    WINDOWED_FULLSCREEN
};

class Window
{
public:
    enum class MouseCursor
    {
	ARROW,
	TEXT,
	RESIZE_ALL,
	RESIZE_VERTICAL,
	RESIZE_HORIZONTAL,
	RESIZE_TRBL,
	RESIZE_TLBR,
	HAND,
	CROSSHAIR
    };

    explicit Window(WindowHandle handle, void* rawHandle, const char* title, unsigned int x, unsigned int y, unsigned int width, unsigned int height);

    const char* GetTitle();

    WindowHandle GetHandle();
    void* GetRawHandle();

    Keyboard& GetKeyboard();
    Mouse& GetMouse();

    void SetFocused(bool focused);
    bool IsFocused() const;

    void SetMouseCursor(MouseCursor cursor);
    MouseCursor GetMouseCursor();

    void SetWindowMode(WindowMode mode);
    WindowMode GetWindowMode();

    unsigned int GetPositionX() const;
    unsigned int GetPositionY() const;
    unsigned int GetWidth() const;
    unsigned int GetHeight() const;

private:
    unsigned int m_positionX;
    unsigned int m_positionY;
    unsigned int m_windowWidth;
    unsigned int m_windowHeight;
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_windowedWidth;
    unsigned int m_windowedHeight;

    char m_title[256];
    bool m_isFocused;
    MouseCursor m_cursor;
    WindowMode m_mode;

    WindowHandle m_handle;
    void* m_rawHandle;

    Keyboard m_keyboard;
    Mouse m_mouse;
};
