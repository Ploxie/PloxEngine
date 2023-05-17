//
// Created by Ploxie on 2023-03-22.
//

#pragma once
#include "eastl/string.h"
#include "Enum.h"
#include <malloc.h>

#define STACK_ALLOC(numBytes)	   alloca(numBytes)
#define STACK_ALLOC_T(type, count) ((type *) alloca((count) * sizeof(type)))

#define CLAMP(value, min, max) ((value <= min) ? min : (value >= max) ? max : \
									value)
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

namespace Util
{
    void FatalExit(const char *message, int exitCode);

    template<class T>
    inline void HashCombine(size_t &s, const T &v)
    {
	eastl::hash<T> h;
	s ^= h(v) + 0x9e3779b9 + (s << 6) + (s >> 2);
    }
} // namespace Util
