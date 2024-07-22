#pragma once

#include <memory>

#include "Base.h"
#include "spdlog/spdlog.h"

namespace Echo 
{
	class Log 
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

#define EC_CORE_ERROR(...) ::Echo::Log::GetCoreLogger()->error(__VA_ARGS__)
#define EC_CORE_WARN(...) ::Echo::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define EC_CORE_INFO(...) ::Echo::Log::GetCoreLogger()->info(__VA_ARGS__)
#define EC_CORE_TRACE(...) ::Echo::Log::GetCoreLogger()->trace(__VA_ARGS__)

#define EC_ERROR(...) ::Echo::Log::GetClientLogger()->error(__VA_ARGS__)
#define EC_WARN(...) ::Echo::Log::GetClientLogger()->warn(__VA_ARGS__)
#define EC_INFO(...) ::Echo::Log::GetClientLogger()->info(__VA_ARGS__)
#define EC_TRACE(...) ::Echo::Log::GetClientLogger()->trace(__VA_ARGS__)