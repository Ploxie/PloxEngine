//
// Created by Ploxie on 2023-03-22.
//

#pragma once

struct GLFWwindow;
struct GLFWcursor;

class Window
{
	friend void WindowSizeCallback(GLFWwindow* window, int width, int height);
	friend void FrameBufferSizeCallback(GLFWwindow* window, int width, int height);
	friend void CursorPosCallback(GLFWwindow* window, double x, double y);
	friend void CursorEnterCallback(GLFWwindow* window, int entered);
	friend void ScrollCallback(GLFWwindow* window, double xOffset, double yOffset);
	friend void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	friend void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void CharCallback(GLFWwindow* window, unsigned int codepoint);

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

	enum class MouseCursorMode
	{
		NORMAL,
		HIDDEN,
		DISABLED
	};
	enum class WindowMode
	{
		WINDOWED,
		FULL_SCREEN
	};

	explicit Window(unsigned int width, unsigned int height, WindowMode mode, const char* title);

	~Window();

	void PollEvents();

	void* GetHandle() const;
	void* GetHandleRaw() const;
	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetWindowWidth() const;
	unsigned int GetWindowHeight() const;
	bool ShouldClose() const;

	void SetMouseCursorMode(MouseCursorMode mode);
	MouseCursorMode GetMouseCursorMode() const;
	void SetWindowMode(WindowMode windowMode);
	WindowMode GetWindowMode() const;
	void SetTitle(const char* title);
	void SetMouseCursor(MouseCursor cursor);
	void SetCursorPosition(float x, float y);
	bool IsFocused() const;

private:
	GLFWwindow* m_handle;
	GLFWcursor* m_cursors[9];
	WindowMode m_mode;
	WindowMode m_newMode;
	unsigned int m_windowWidth;
	unsigned int m_windowHeight;
	unsigned int m_width;
	unsigned int m_height;
	unsigned int m_windowedWidth;
	unsigned int m_windowedHeight;
	char m_title[256];
	bool m_configurationChanged;
	MouseCursorMode m_cursorMode = MouseCursorMode::NORMAL;
};
