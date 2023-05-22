//
// Created by Ploxie on 2023-03-22.
//

#include "Utilities.h"
#include "core/logger.h"
#include <windows.h>

namespace Util
{

    void FatalExit(const char* message, int exitCode)
    {
	LOG_CORE_ERROR(message);
	MessageBoxA(nullptr, message, nullptr, MB_OK | MB_ICONERROR);
	exit(exitCode);
    }

    uint32_t FindFirstSetBit(uint32_t mask)
    {
	uint32_t r = 1;

	if (!mask)
	{
	    return UINT32_MAX;
	}

	if (!(mask & 0xFFFF))
	{
	    mask >>= 16;
	    r += 16;
	}
	if (!(mask & 0xFF))
	{
	    mask >>= 8;
	    r += 8;
	}
	if (!(mask & 0xF))
	{
	    mask >>= 4;
	    r += 4;
	}
	if (!(mask & 3))
	{
	    mask >>= 2;
	    r += 2;
	}
	if (!(mask & 1))
	{
	    mask >>= 1;
	    r += 1;
	}
	return r - 1;
    }

    uint32_t FindLastSetBit(uint32_t mask)
    {
	uint32_t r = 32;

	if (!mask)
	{
	    return UINT32_MAX;
	}

	if (!(mask & 0xFFFF0000u))
	{
	    mask <<= 16;
	    r -= 16;
	}
	if (!(mask & 0xFF000000u))
	{
	    mask <<= 8;
	    r -= 8;
	}
	if (!(mask & 0xF0000000u))
	{
	    mask <<= 4;
	    r -= 4;
	}
	if (!(mask & 0xC0000000u))
	{
	    mask <<= 2;
	    r -= 2;
	}
	if (!(mask & 0x80000000u))
	{
	    mask <<= 1;
	    r -= 1;
	}
	return r - 1;
    }

} // namespace Util