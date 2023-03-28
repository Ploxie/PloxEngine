//
// Created by Ploxie on 2023-03-23.
//

#include "Platform_Win32.h"
#ifdef _WIN32
#	include "Platform.h"
#	include "utility/utilities.h"
#	include <windows.h>
#	include "core/logger.h"

IPlatform* Platform::s_instance = new PlatformWin32;
const FileSystem* const Platform::FileSystem = Platform::s_instance;

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

bool PlatformWin32::Exists(const Path& path) const
{
	const size_t pathLen = strlen(path);
	auto* pathW = STACK_ALLOC_T(wchar_t, pathLen + 1);

	if(!ConvertToWString(path, pathLen + 1, pathW))
	{
		return false;
	}

	DWORD attributes = GetFileAttributesW(pathW);

	return (attributes != INVALID_FILE_ATTRIBUTES);
}
bool PlatformWin32::IsDirectory(const Path& path) const
{
	const size_t pathLen = strlen(path);
	auto* pathW = STACK_ALLOC_T(wchar_t, pathLen + 1);

	if(!ConvertToWString(path, pathLen + 1, pathW))
	{
		return false;
	}

	DWORD attributes = GetFileAttributesW(pathW);

	return (attributes != INVALID_FILE_ATTRIBUTES) &&
		   ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
}

bool PlatformWin32::IsFile(const Path& path) const
{
	const size_t pathLen = strlen(path);
	auto* pathW = STACK_ALLOC_T(wchar_t, pathLen + 1);

	if(!ConvertToWString(path, pathLen + 1, pathW))
	{
		return false;
	}

	DWORD attributes = GetFileAttributesW(pathW);

	return (attributes != INVALID_FILE_ATTRIBUTES) &&
		   ((attributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

bool PlatformWin32::CreateDirectories(const Path& path) const
{
	const size_t pathLen = strlen(path);
	auto* pathW = STACK_ALLOC_T(wchar_t, pathLen + 1);

	if(!ConvertToWString(path, pathLen + 1, pathW))
	{
		return false;
	}

	const wchar_t* separator = L"\\/";

	wchar_t folder[MAX_PATH] = {};
	wchar_t* endOfString = wcschr(pathW, L'\0');
	wchar_t* end = wcspbrk(pathW, separator);
	end = (end != nullptr) ? end : endOfString;

	while(end != nullptr)
	{
		wcsncpy_s(folder, pathW, end - pathW + 1);

		bool needToCreate;
		{
			DWORD attributes = GetFileAttributesW(folder);
			bool exists = attributes != INVALID_FILE_ATTRIBUTES;
			bool isDir = exists && (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
			needToCreate = !exists || !isDir;
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

bool PlatformWin32::Rename(const Path& path, const char* newName) const
{
	const size_t pathLen = strlen(path);
	auto* pathW = STACK_ALLOC_T(wchar_t, pathLen + 1);

	if(!ConvertToWString(path, pathLen + 1, pathW))
	{
		return false;
	}

	char newPath[MAX_PATH];
	strcpy_s(newPath, path);
	char* parent = max(strrchr(newPath, '\\'), strrchr(newPath, '/'));
	parent[1] = '\0';
	strcat_s(newPath, newName);

	const size_t newPathLen = strlen(newPath);
	wchar_t* newPathW = STACK_ALLOC_T(wchar_t, newPathLen + 1);
	if(!ConvertToWString(newPath, newPathLen + 1, newPathW))
	{
		return false;
	}

	return MoveFileW(pathW, newPathW) != 0;
}

bool PlatformWin32::Remove(const Path& path) const
{
	const size_t pathLen = strlen(path);
	auto* pathW = STACK_ALLOC_T(wchar_t, pathLen + 1);

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

FileHandle PlatformWin32::Open(const Path& path, FileMode mode, bool binary) noexcept
{
	const size_t filePathLen = strlen(path);
	if((filePathLen +1) > Path::MAX_PATH_LENGTH)
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

		OpenFile openFile{};
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

uint64_t PlatformWin32::Read(FileHandle fileHandle, size_t bufferSize, void* buffer) const noexcept
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

bool PlatformWin32::ReadFile(const Path& path,
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

uint64_t PlatformWin32::Write(FileHandle fileHandle, size_t bufferSize, const void* buffer) const noexcept
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

bool PlatformWin32::WriteFile(const Path& path,
							  size_t bufferSize,
							  const void* buffer,
							  bool binary) noexcept
{

	auto fileHandle = Open(path, FileMode::WRITE, binary);
	if(fileHandle != NULL_FILE_HANDLE)
	{
		uint64_t writeCount = Write(fileHandle,  bufferSize, buffer);
		Close(fileHandle);
		return writeCount == bufferSize;
	}

	return false;
}

void PlatformWin32::Close(FileHandle fileHandle) noexcept
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

uint64_t PlatformWin32::Size(const Path& path) const
{
	const size_t pathLen = strlen(path);
	auto* pathW = STACK_ALLOC_T(wchar_t, pathLen + 1);

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

#endif