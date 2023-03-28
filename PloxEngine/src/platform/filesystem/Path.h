//
// Created by Ploxie on 2023-03-24.
//

#pragma once

class Path
{
public:
	static constexpr size_t MAX_PATH_LENGTH = 256;
	static constexpr const char* SEPARATORS = "/\\";

public:
	Path() = default;
	Path(char const* path) noexcept;

	const char* GetExtension() noexcept;
	const char* GetFileName() noexcept;
	Path GetParentPath() noexcept;

	operator const char*() const;

private:
	char m_str[MAX_PATH_LENGTH];
};
