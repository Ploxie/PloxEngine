//
// Created by Ploxie on 2023-03-24.
//

#include "Path.h"
#include <cstring>
#include <minmax.h>

Path::Path(char const* path) noexcept
{
	strncpy_s(m_str, path, strlen(path));
}

const char* Path::GetExtension() noexcept
{
	return strrchr(m_str, '.');
}

const char* Path::GetFileName() noexcept
{
	return max(strrchr(m_str, '/'), strrchr(m_str, '\\'));
}
Path Path::GetParentPath() noexcept
{
	char parent[MAX_PATH_LENGTH];

	size_t parentPathEnd = 0;
	size_t curOffset = 0;

	while (m_str[curOffset] != '\0')
	{
		for(size_t i = 0; i < strlen(SEPARATORS); i++)
		{
			if(m_str[curOffset] == SEPARATORS[i])
			{
				parentPathEnd = curOffset;
			}
		}
		curOffset++;
	}

	strncpy_s(parent, m_str, parentPathEnd);
	Path result = parent;
	return result;
}
Path::operator const char*() const
{
	return m_str;
}
