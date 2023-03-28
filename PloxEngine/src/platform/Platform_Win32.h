//
// Created by Ploxie on 2023-03-23.
//

#pragma once
#include "Platform.h"
#include <cstddef>
#include <cstdint>
#include "filesystem/FileSystem.h"

class PlatformWin32 : public IPlatform
{
public:
	explicit PlatformWin32() = default;

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

private:
};

