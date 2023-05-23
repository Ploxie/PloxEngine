//
// Created by Ploxie on 2023-03-23.
//

#pragma once
#include "eastl/vector.h"
#include "platform/filesystem/FileSystem.h"
#include "window/window.h"
#include <cstdint>

class PlatformFileSystem : public FileSystem
{
public:
    bool Exists(const Path& path) const override;
    bool IsDirectory(const Path& path) const override;
    bool IsFile(const Path& path) const override;
    bool CreateDirectories(const Path& path) const override;
    bool Rename(const Path& path, const char* newName) const override;
    bool Remove(const Path& path) const override;

    FileHandle Open(const Path& path, FileMode mode, bool binary) noexcept override;

    uint64_t Read(FileHandle fileHandle, size_t bufferSize, void* buffer) const noexcept override;
    bool ReadFile(const Path& path, size_t bufferSize, void* buffer, bool binary) noexcept override;

    uint64_t Write(FileHandle fileHandle, size_t bufferSize, const void* buffer) const noexcept override;
    bool WriteFile(const Path& path, size_t bufferSize, const void* buffer, bool binary) noexcept override;

    void Close(FileHandle fileHandle) noexcept override;
    uint64_t Size(const Path& path) const override;
};

class Platform
{
public:
    static bool Initialize(const char* applicationName);
    static WindowHandle CreatePlatformWindow(const char* title, int x, int y, int width, int height);

    static void* GetApplicationInstance();

    static Window* GetWindow(WindowHandle handle);
    static Window* GetWindow(void* rawHandle);
    static WindowHandle GetActiveWindow();
    static bool DestroyWindow(WindowHandle handle);

    static void SetCursor(Window::MouseCursor cursor);

    static bool PumpMessages();

public:
    // FileSystem
    static bool Exists(const Path& path)
    {
	return s_instance.m_fileSystem.Exists(path);
    }
    static bool IsDirectory(const Path& path)
    {
	return s_instance.m_fileSystem.IsDirectory(path);
    }
    static bool IsFile(const Path& path)
    {
	return s_instance.m_fileSystem.IsFile(path);
    }
    static bool CreateDirectories(const Path& path)
    {
	return s_instance.m_fileSystem.CreateDirectories(path);
    }
    static bool Rename(const Path& path, const char* newName)
    {
	return s_instance.m_fileSystem.Rename(path, newName);
    }
    static bool Remove(const Path& path)
    {
	return s_instance.m_fileSystem.Remove(path);
    }
    static FileHandle Open(const Path& path, FileMode mode, bool binary) noexcept
    {
	return s_instance.m_fileSystem.Open(path, mode, binary);
    }
    static uint64_t Read(FileHandle fileHandle, size_t bufferSize, void* buffer) noexcept
    {
	return s_instance.m_fileSystem.Read(fileHandle, bufferSize, buffer);
    }
    static bool ReadFile(const Path& path, size_t bufferSize, void* buffer, bool binary) noexcept
    {
	return s_instance.m_fileSystem.ReadFile(path, bufferSize, buffer, binary);
    }
    static uint64_t Write(FileHandle fileHandle, size_t bufferSize, const void* buffer) noexcept
    {
	return s_instance.m_fileSystem.Write(fileHandle, bufferSize, buffer);
    }
    static bool WriteFile(const Path& path, size_t bufferSize, const void* buffer, bool binary) noexcept
    {
	return s_instance.m_fileSystem.WriteFile(path, bufferSize, buffer, binary);
    }
    static void Close(FileHandle fileHandle) noexcept
    {
	return s_instance.m_fileSystem.Close(fileHandle);
    }
    static uint64_t Size(const Path& path)
    {
	return s_instance.m_fileSystem.Size(path);
    }

private:
    static Platform s_instance;

    PlatformFileSystem m_fileSystem;
    char m_applicationName[256];
    void* m_applicationInstance;

    eastl::vector<Window> m_windows;
    HandleManager m_windowHandleManager;

    void* m_cursors[9];
};
