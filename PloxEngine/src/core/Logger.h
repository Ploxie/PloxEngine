//
// Created by Ploxie on 2023-03-21.
//

#pragma once
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <memory>

using logger = std::shared_ptr<spdlog::logger>;

class Logger
{
public:
	static void Initialize();
	inline static logger& GetCoreLogger()
	{
		return m_coreLogger;
	}
	inline static logger& GetClientLogger()
	{
		return m_clientLogger;
	}

private:
	static logger m_coreLogger;
	static logger m_clientLogger;
};

#define LOG_CORE_CRITICAL(...) ::Logger::GetCoreLogger()->critical(__VA_ARGS__)
#define LOG_CORE_ERROR(...) ::Logger::GetCoreLogger()->error(__VA_ARGS__)
#define LOG_CORE_WARN(...) ::Logger::GetCoreLogger()->warn(__VA_ARGS__)
#define LOG_CORE_INFO(...) ::Logger::GetCoreLogger()->info(__VA_ARGS__)
#define LOG_CORE_TRACE(...) ::Logger::GetCoreLogger()->trace(__VA_ARGS__)

#define LOG_CRITICAL(...) ::Logger::GetClientLogger()->critical(__VA_ARGS__)
#define LOG_ERROR(...) ::Logger::GetClientLogger()->error(__VA_ARGS__)
#define LOG_WARN(...) ::Logger::GetClientLogger()->warn(__VA_ARGS__)
#define LOG_INFO(...) ::Logger::GetClientLogger()->info(__VA_ARGS__)
#define LOG_TRACE(...) ::Logger::GetClientLogger()->trace(__VA_ARGS__)