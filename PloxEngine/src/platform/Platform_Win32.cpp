//
// Created by Ploxie on 2023-03-23.
//

#ifdef _WIN32
    #include "core/engine.h"
    #include "core/logger.h"
    #include "Platform.h"
    #include "utility/Utilities.h"
    #include <windows.h>
    #include <windowsx.h>

    #undef CreateSemaphore

Platform Platform::s_instance;

LRESULT CALLBACK Win32_process_message(HWND hwnd, unsigned int msg, WPARAM w_param, LPARAM l_param);

bool Platform::Initialize(const char* applicationName)
{
    s_instance.m_applicationInstance = GetModuleHandleA(nullptr);

    s_instance.m_cursors[(size_t) Window::MouseCursor::ARROW]		  = LoadCursor(nullptr, IDC_ARROW);
    s_instance.m_cursors[(size_t) Window::MouseCursor::TEXT]		  = LoadCursor(nullptr, IDC_IBEAM);
    s_instance.m_cursors[(size_t) Window::MouseCursor::RESIZE_ALL]	  = LoadCursor(nullptr, IDC_SIZEALL);
    s_instance.m_cursors[(size_t) Window::MouseCursor::RESIZE_VERTICAL]	  = LoadCursor(nullptr, IDC_SIZENS);
    s_instance.m_cursors[(size_t) Window::MouseCursor::RESIZE_HORIZONTAL] = LoadCursor(nullptr, IDC_SIZEWE);
    s_instance.m_cursors[(size_t) Window::MouseCursor::RESIZE_TRBL]	  = LoadCursor(nullptr, IDC_SIZENESW);
    s_instance.m_cursors[(size_t) Window::MouseCursor::RESIZE_TLBR]	  = LoadCursor(nullptr, IDC_SIZENWSE);
    s_instance.m_cursors[(size_t) Window::MouseCursor::HAND]		  = LoadCursor(nullptr, IDC_HAND);
    s_instance.m_cursors[(size_t) Window::MouseCursor::CROSSHAIR]	  = LoadCursor(nullptr, IDC_CROSS);

    return true;
}

WindowHandle Platform::CreatePlatformWindow(const char* title, unsigned int x, unsigned int y, unsigned int width, unsigned int height)
{
    auto* instance = static_cast<HINSTANCE>(s_instance.m_applicationInstance);

    char className[256] = "\0";
    strcat_s(className, title);
    strcat_s(className, "_class\0");

    HICON icon = LoadIcon(instance, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style	     = CS_DBLCLKS;
    wc.lpfnWndProc   = Win32_process_message;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = instance;
    wc.hIcon	     = icon;
    wc.hCursor	     = LoadCursor(nullptr, IDC_HAND);
    wc.hbrBackground = nullptr;
    wc.lpszClassName = className;

    if(RegisterClassA(&wc) == 0)
    {
	LOG_CRITICAL("PlatformWin32: Failed to register class");
	return NULL_WINDOW_HANDLE;
    }

    int clientX	     = x;
    int clientY	     = y;
    int clientWidth  = width;
    int clientHeight = height;

    int windowX	     = clientX;
    int windowY	     = clientY;
    int windowWidth  = clientWidth;
    int windowHeight = clientHeight;

    unsigned int windowStyle	     = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    unsigned int windowStyleExtended = WS_EX_APPWINDOW;

    //windowStyle |= WS_MAXIMIZEBOX;
    //windowStyle |= WS_MINIMIZEBOX;
    //windowStyle |= WS_THICKFRAME;

    RECT borderRect = { 0, 0, 0, 0 };
    AdjustWindowRectEx(&borderRect, windowStyle, 0, windowStyleExtended);

    windowX += borderRect.left;
    windowY += borderRect.top;

    HWND hwnd = CreateWindowExA(windowStyleExtended, className, title, windowStyle, windowX, windowY, windowWidth, windowHeight, nullptr, nullptr, instance, nullptr);

    if(hwnd == nullptr)
    {
	LOG_CRITICAL("PlatformWin32: Failed to create window!");
	return NULL_WINDOW_HANDLE;
    }

    auto handle = static_cast<WindowHandle>(s_instance.m_windowHandleManager.Allocate());
    Window window(handle, hwnd, title, x, y, width, height);
    s_instance.m_windows.push_back(window);

    ShowWindow(hwnd, SW_SHOW);

    return handle;
}

void* Platform::GetApplicationInstance()
{
    return s_instance.m_applicationInstance;
}

Window* Platform::GetWindow(WindowHandle handle)
{
    if(handle == NULL_WINDOW_HANDLE)
    {
	return nullptr;
    }
    return &s_instance.m_windows[handle - 1];
}

Window* Platform::GetWindow(void* rawHandle)
{
    for(auto& window: s_instance.m_windows)
    {
	if(window.GetRawHandle() == rawHandle)
	{
	    return &window;
	}
    }
    return nullptr;
}

WindowHandle Platform::GetActiveWindow()
{
    for(auto& window: s_instance.m_windows)
    {
	if(window.IsFocused())
	{
	    return window.GetHandle();
	}
    }

    return NULL_WINDOW_HANDLE;
}

bool Platform::DestroyWindow(WindowHandle handle)
{
    Window* window = GetWindow(handle);

    bool destroyed = ::DestroyWindow(static_cast<HWND>(window->GetRawHandle())) != 0;
    s_instance.m_windows.erase(window);
    return destroyed;
}

void Platform::SetCursor(Window::MouseCursor cursor)
{
    ::SetCursor(static_cast<HCURSOR>(s_instance.m_cursors[static_cast<size_t>(cursor)]));
}

bool Platform::PumpMessages()
{
    MSG message;
    for(auto window: s_instance.m_windows)
    {
	Mouse& mouse = window.GetMouse();
	mouse.Update();

	auto* windowHandle = static_cast<HWND>(window.GetRawHandle());
	while(PeekMessageA(&message, windowHandle, 0, 0, PM_REMOVE) != 0)
	{
	    TranslateMessage(&message);
	    DispatchMessage(&message);
	}
    }

    return !s_instance.m_windows.empty();
}

LRESULT CALLBACK Win32_process_message(HWND handle, unsigned int msg, WPARAM w_param, LPARAM l_param)
{
    Window* window     = Platform::GetWindow(static_cast<void*>(handle));
    Keyboard& keyboard = window->GetKeyboard();
    Mouse& mouse       = window->GetMouse();

    switch(msg)
    {
	case WM_SETCURSOR:
	{
	    if(LOWORD(l_param) == HTCLIENT)
	    {
		Platform::SetCursor(window->GetMouseCursor());
		return 0;
	    }
	}
	break;
	case WM_CLOSE:
	{
	    Platform::DestroyWindow(window->GetHandle());
	    LOG_WARN("Closing window: {0}", window->GetTitle());
	}
	break;
	case WM_ACTIVATE:
	{
	    bool active = LOWORD(w_param) == WA_ACTIVE || LOWORD(w_param) == WA_CLICKACTIVE;
	    window->SetFocused(active);
	}
	break;
	case WM_CHAR:
	{
	}
	break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
	{
	    // Key pressed/released
	    bool pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
	    bool repeat	 = (l_param & 0x40000000) != 0;
	    Key key	 = static_cast<Key>(w_param);

	    bool isExtended = (HIWORD(l_param) & KF_EXTENDED) == KF_EXTENDED;

	    if(w_param == VK_MENU)
	    {
		key = isExtended ? Key::RIGHT_ALT : Key::LEFT_ALT;
	    }
	    else if(w_param == VK_SHIFT)
	    {
		unsigned int leftShift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
		unsigned int scanCode  = ((l_param & (0xFF << 16)) >> 16);
		key		       = scanCode == leftShift ? Key::LEFT_SHIFT : Key::RIGHT_SHIFT;
	    }
	    else if(w_param == VK_CONTROL)
	    {
		key = isExtended ? Key::RIGHT_CONTROL : Key::LEFT_CONTROL;
	    }

	    int action = repeat && pressed ? static_cast<int>(InputAction::REPEAT) : static_cast<int>(pressed);
	    keyboard.OnKey(key, static_cast<InputAction>(action));
	}
	break;
	case WM_MOUSEMOVE:
	{
	    // Mouse move
	    unsigned int x = GET_X_LPARAM(l_param);
	    unsigned int y = GET_Y_LPARAM(l_param);

	    mouse.OnMove(x, y);
	}
	break;
	case WM_MOUSEWHEEL:
	{
	    int z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
	    if(z_delta != 0)
	    {
		// Flatten the input to an OS-independent (-1, 1)
		z_delta = (z_delta < 0) ? -1 : 1;
		mouse.OnScroll(0, z_delta);
	    }
	}
	break;
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	{
	    bool pressed = msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN;
	    MouseButton mouse_button;
	    switch(msg)
	    {
		case WM_LBUTTONDOWN:
		case WM_LBUTTONUP: mouse_button = MouseButton::BUTTON_LEFT; break;
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP: mouse_button = MouseButton::BUTTON_MIDDLE; break;
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP: mouse_button = MouseButton::BUTTON_RIGHT; break;
	    }

	    auto action = static_cast<InputAction>(pressed);
	    mouse.OnButton(mouse_button, action);
	}
	break;
	default:
	    break;
    }

    return DefWindowProcA(handle, msg, w_param, l_param);
}

#endif