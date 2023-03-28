//
// Created by Ploxie on 2023-03-22.
//

#pragma once
#include <malloc.h>

#define STACK_ALLOC(numBytes) alloca(numBytes)
#define STACK_ALLOC_T(type, count) ((type *)alloca((count) * sizeof(type)))

namespace Util
{
    void FatalExit(const char* message, int exitCode);
}
