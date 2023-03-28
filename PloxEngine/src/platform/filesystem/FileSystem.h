//
// Created by Ploxie on 2023-03-24.
//

#pragma once
#include "Path.h"
#include "eastl/vector.h"
#include "utility/HandleManager.h"
#include "utility/spinlock.h"

enum FileHandle : size_t
{
	NULL_FILE_HANDLE
};

enum class FileMode
{
	READ,
	WRITE,
	APPEND,
	OPEN_READ_WRITE,
	CREATE_READ_WRITE,
	APPEND_OR_CREATE_READ_WRITE,
};

class FileSystem
{
public:
	virtual bool Exists(const Path& path) const = 0;
	virtual bool IsDirectory(const Path& path) const = 0;
	virtual bool IsFile(const Path& path) const = 0;
	virtual bool CreateDirectories(const Path& path) const = 0;
	virtual bool Rename(const Path& path, const char* newName) const = 0;
	virtual bool Remove(const Path& path) const = 0;
	virtual FileHandle Open(const Path& path, FileMode mode, bool binary) noexcept = 0;
	virtual uint64_t Read(FileHandle fileHandle, size_t bufferSize, void* buffer) const noexcept = 0;
	virtual bool
	ReadFile(const Path& path, size_t bufferSize, void* buffer, bool binary) noexcept = 0;
	virtual uint64_t Write(FileHandle fileHandle, size_t bufferSize, const void* buffer) const noexcept = 0;
	virtual bool WriteFile(const Path& path,
						   size_t bufferSize,
						   const void* buffer,
						   bool binary) noexcept = 0;
	virtual void Close(FileHandle fileHandle) noexcept = 0;
	virtual uint64_t Size(const Path& path) const = 0;

protected:
	struct OpenFile
	{
		Path m_path;
		void* m_file;
	};

	eastl::vector<OpenFile> m_openFiles;
	HandleManager m_openFileHandleManager;

	mutable SpinLock m_openFilesSpinLock;
};
