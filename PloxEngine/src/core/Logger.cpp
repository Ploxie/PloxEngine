//
// Created by Ploxie on 2023-03-21.
//

#include "Logger.h"

logger Logger::m_coreLogger;
logger Logger::m_clientLogger;

void ReportAssertionFailure(const char* expression, const char* message, const char* file, unsigned int line)
{
	LOG_CRITICAL("Assertion Failure: {0}, message: '{1}', in file: {2}, line {3}",expression, message, file, line);
}

void Logger::Initialize()
{
    spdlog::set_pattern("%^[%T][%n]: %v%$");

    m_coreLogger = spdlog::stdout_color_mt("Engine");
    m_coreLogger->set_level(spdlog::level::trace);

    m_clientLogger = spdlog::stdout_color_mt("Client");
    m_clientLogger->set_level(spdlog::level::trace);
}