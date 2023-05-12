//
// Created by Ploxie on 2023-03-23.
//

#ifdef _WIN32
    #include "core/engine.h"
    #include "core/logger.h"
    #include "Platform.h"
    #include "rendering/vulkan/vulkangraphicsadapter.h"
    #include "utility/utilities.h"
    #include <vulkan/vulkan_win32.h>
    #include <windows.h>
    #include <windowsx.h>

    #undef CreateSemaphore

Platform Platform::s_instance;

LRESULT CALLBACK Win32_process_message(HWND hwnd, unsigned int msg, WPARAM w_param, LPARAM l_param);

bool ConvertToWString(const char* string, size_t bufferSize, wchar_t* result)
{
    if((string == nullptr) || (result == nullptr))
    {
	return false;
    }

    int requiredSize = MultiByteToWideChar(CP_UTF8, 0, string, -1, nullptr, 0);

    if(requiredSize > bufferSize)
    {
	return false;
    }

    int writtenSize = MultiByteToWideChar(CP_UTF8, 0, string, -1, result, requiredSize);

    return requiredSize == writtenSize;
}

bool Platform::Initialize(const char* applicationName)
{
    s_instance.m_applicationInstance = GetModuleHandleA(nullptr);

    auto* instance = static_cast<HINSTANCE>(s_instance.m_applicationInstance);

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
    ::SetCursor(static_cast<HCURSOR>(s_instance.m_cursors[(size_t) cursor]));
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

bool PlatformFileSystem::Exists(const Path& path) const
{
    const size_t pathLen = strlen(path);
    auto* pathW		 = STACK_ALLOC_T(wchar_t, pathLen + 1);

    if(!ConvertToWString(path, pathLen + 1, pathW))
    {
	return false;
    }

    DWORD attributes = GetFileAttributesW(pathW);

    return (attributes != INVALID_FILE_ATTRIBUTES);
}
bool PlatformFileSystem::IsDirectory(const Path& path) const
{
    const size_t pathLen = strlen(path);
    auto* pathW		 = STACK_ALLOC_T(wchar_t, pathLen + 1);

    if(!ConvertToWString(path, pathLen + 1, pathW))
    {
	return false;
    }

    DWORD attributes = GetFileAttributesW(pathW);

    return (attributes != INVALID_FILE_ATTRIBUTES) &&
	   ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool PlatformFileSystem::IsFile(const Path& path) const
{
    const size_t pathLen = strlen(path);
    auto* pathW		 = STACK_ALLOC_T(wchar_t, pathLen + 1);

    if(!ConvertToWString(path, pathLen + 1, pathW))
    {
	return false;
    }

    DWORD attributes = GetFileAttributesW(pathW);

    return (attributes != INVALID_FILE_ATTRIBUTES) &&
	   ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool PlatformFileSystem::CreateDirectories(const Path& path) const
{
    const size_t pathLen = strlen(path);
    auto* pathW		 = STACK_ALLOC_T(wchar_t, pathLen + 1);

    if(!ConvertToWString(path, pathLen + 1, pathW))
    {
	return false;
    }

    const wchar_t* separator = L"\\/";

    wchar_t folder[MAX_PATH] = {};
    wchar_t* endOfString     = wcschr(pathW, L'\0');
    wchar_t* end	     = wcspbrk(pathW, separator);
    end			     = (end != nullptr) ? end : endOfString;

    while(end != nullptr)
    {
	wcsncpy_s(folder, pathW, end - pathW + 1);

	bool needToCreate;
	{
	    DWORD attributes = GetFileAttributesW(folder);
	    bool exists	     = attributes != INVALID_FILE_ATTRIBUTES;
	    bool isDir	     = exists && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
	    needToCreate     = !exists || !isDir;
	}

	if(needToCreate && (CreateDirectoryW(folder, nullptr) == 0))
	{
	    DWORD error = GetLastError();
	    if(error != ERROR_ALREADY_EXISTS)
	    {
		LOG_WARN("PlatformWin32: Failed to create directory! Error: {0}", error);
		return false;
	    }
	}

	if(end == endOfString)
	{
	    break;
	}
	++end;
	end = wcspbrk(end, separator);
	end = (end != nullptr) ? end : endOfString;
    }

    return true;
}

bool PlatformFileSystem::Rename(const Path& path, const char* newName) const
{
    const size_t pathLen = strlen(path);
    auto* pathW		 = STACK_ALLOC_T(wchar_t, pathLen + 1);

    if(!ConvertToWString(path, pathLen + 1, pathW))
    {
	return false;
    }

    char newPath[MAX_PATH];
    strcpy_s(newPath, path);
    char* parent = MAX(strrchr(newPath, '\\'), strrchr(newPath, '/'));
    parent[1]	 = '\0';
    strcat_s(newPath, newName);

    const size_t newPathLen = strlen(newPath);
    wchar_t* newPathW	    = STACK_ALLOC_T(wchar_t, newPathLen + 1);
    if(!ConvertToWString(newPath, newPathLen + 1, newPathW))
    {
	return false;
    }

    return MoveFileW(pathW, newPathW) != 0;
}

bool PlatformFileSystem::Remove(const Path& path) const
{
    const size_t pathLen = strlen(path);
    auto* pathW		 = STACK_ALLOC_T(wchar_t, pathLen + 1);

    if(!ConvertToWString(path, pathLen + 1, pathW))
    {
	return false;
    }

    DWORD attributes = GetFileAttributesW(pathW);

    if(attributes == INVALID_FILE_ATTRIBUTES)
    {
	return false;
    }

    if((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
    {
	return DeleteFileW(pathW) != 0;
    }

    return RemoveDirectoryW(pathW) != 0;
}

FileHandle PlatformFileSystem::Open(const Path& path, FileMode mode, bool binary) noexcept
{
    const size_t filePathLen = strlen(path);
    if((filePathLen + 1) > Path::MAX_PATH_LENGTH)
    {
	return NULL_FILE_HANDLE;
    }

    const char* fileMode;

    switch(mode)
    {
	case FileMode::READ:
	    fileMode = binary ? "rb" : "r";
	    break;
	case FileMode::WRITE:
	    fileMode = binary ? "wb" : "w";
	    break;
	case FileMode::APPEND:
	    fileMode = binary ? "ab" : "a";
	    break;
	case FileMode::OPEN_READ_WRITE:
	    fileMode = binary ? "r+b" : "r+";
	    break;
	case FileMode::CREATE_READ_WRITE:
	    fileMode = binary ? "w+b" : "w+";
	    break;
	case FileMode::APPEND_OR_CREATE_READ_WRITE:
	    fileMode = binary ? "a+b" : "a+";
	    break;
	default:
	    return NULL_FILE_HANDLE;
    }

    FILE* file = nullptr;
    errno_t err;
    if((err = fopen_s(&file, path, fileMode)) != 0)
    {
	char buf[256];
	strerror_s(buf, err);
	LOG_WARN("Failed to open file {0}: {1}", path, buf);
	return NULL_FILE_HANDLE;
    }

    FileHandle resultHandle;
    {
	SpinLockHolder spinLock(m_openFilesSpinLock);
	resultHandle = static_cast<FileHandle>(m_openFileHandleManager.Allocate());

	if(resultHandle == NULL_FILE_HANDLE)
	{
	    fclose(file);
	    return NULL_FILE_HANDLE;
	}

	OpenFile openFile {};
	memcpy(&openFile.m_path, path, filePathLen + 1);
	openFile.m_file = file;

	const auto idx = static_cast<size_t>(resultHandle - 1);

	if(m_openFiles.size() <= idx)
	{
	    size_t newSize = idx;
	    newSize += eastl::max<size_t>(1, newSize / 2);
	    newSize = eastl::max<size_t>(16, newSize);
	    m_openFiles.resize(newSize);
	}

	m_openFiles[idx] = openFile;
    }

    return resultHandle;
}

uint64_t PlatformFileSystem::Read(FileHandle fileHandle, size_t bufferSize, void* buffer) const noexcept
{
    if(fileHandle == NULL_FILE_HANDLE)
    {
	return 0;
    }

    SpinLockHolder spinLock(m_openFilesSpinLock);

    FILE* file = static_cast<FILE*>(m_openFiles[fileHandle - 1].m_file);

    if(file != nullptr)
    {
	return fread(buffer, 1, bufferSize, file);
    }

    return 0;
}

bool PlatformFileSystem::ReadFile(const Path& path,
				  size_t bufferSize,
				  void* buffer,
				  bool binary) noexcept
{
    auto fileHandle = Open(path, FileMode::READ, binary);
    if(fileHandle != NULL_FILE_HANDLE)
    {
	uint64_t readCount = Read(fileHandle, bufferSize, buffer);
	Close(fileHandle);
	return readCount <= bufferSize;
    }
    return false;
}

uint64_t PlatformFileSystem::Write(FileHandle fileHandle, size_t bufferSize, const void* buffer) const noexcept
{
    if(fileHandle == NULL_FILE_HANDLE)
    {
	return 0;
    }

    SpinLockHolder spinLock(m_openFilesSpinLock);

    FILE* file = static_cast<FILE*>(m_openFiles[fileHandle - 1].m_file);

    if(file != nullptr)
    {
	return fwrite(buffer, 1, bufferSize, file);
    }

    return 0;
}

bool PlatformFileSystem::WriteFile(const Path& path,
				   size_t bufferSize,
				   const void* buffer,
				   bool binary) noexcept
{
    auto fileHandle = Open(path, FileMode::WRITE, binary);
    if(fileHandle != NULL_FILE_HANDLE)
    {
	uint64_t writeCount = Write(fileHandle, bufferSize, buffer);
	Close(fileHandle);
	return writeCount == bufferSize;
    }

    return false;
}

void PlatformFileSystem::Close(FileHandle fileHandle) noexcept
{
    if(fileHandle == NULL_FILE_HANDLE)
    {
	return;
    }

    SpinLockHolder spinLock(m_openFilesSpinLock);

    FILE* file = static_cast<FILE*>(m_openFiles[fileHandle - 1].m_file);

    if(file != nullptr)
    {
	fclose(file);
	m_openFiles[fileHandle - 1] = {};
	m_openFileHandleManager.Free(fileHandle);
    }
}

uint64_t PlatformFileSystem::Size(const Path& path) const
{
    const size_t pathLen = strlen(path);
    auto* pathW		 = STACK_ALLOC_T(wchar_t, pathLen + 1);

    if(!ConvertToWString(path, pathLen + 1, pathW))
    {
	return 0;
    }

    WIN32_FILE_ATTRIBUTE_DATA attributeData;
    if(GetFileAttributesExW(pathW, GetFileExInfoStandard, &attributeData) == 0)
    {
	return 0;
    }

    uint64_t result = 0;
    result |= attributeData.nFileSizeHigh;
    result <<= 32;
    result |= attributeData.nFileSizeLow;

    return result;
}

void Vulkan::AppendPlatformExtensions(eastl::vector<const char*>& extensions)
{
    extensions.push_back("VK_KHR_win32_surface");
}

VkResult Vulkan::CreateWindowSurface(void* windowHandle, VkInstance instance, VkSurfaceKHR* surface)
{
    VkWin32SurfaceCreateInfoKHR create_info = { VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR };
    create_info.hinstance		    = static_cast<HINSTANCE>(Platform::GetApplicationInstance());
    create_info.hwnd			    = static_cast<HWND>(windowHandle);

    return vkCreateWin32SurfaceKHR(instance, &create_info, nullptr, surface);
}

#endif