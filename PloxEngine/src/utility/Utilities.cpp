//
// Created by Ploxie on 2023-03-22.
//

#include "Utilities.h"
#include <windows.h>
#include "core/logger.h"

namespace Util
{

    void FatalExit(const char* message, int exitCode)
    {
        LOG_CORE_ERROR(message);
        MessageBoxA(nullptr, message, nullptr, MB_OK | MB_ICONERROR);
        exit(exitCode);
    }

}