#pragma once

#include "Core/Application.h"
#include "Debug/Instrumentor.h"

#ifdef ECHO_PLATFORM_WIN

extern Echo::Application* Echo::CreateApplication();

int main()
{
	Echo::Log::Init();
	EC_PROFILE_BEGIN_SESSION("Startup", "EchoProfile-Startup.json");
	auto app = Echo::CreateApplication();
	EC_PROFILE_END_SESSION();

	EC_PROFILE_BEGIN_SESSION("Runtime", "EchoProfile-Runtime.json");
	app->Run();
	EC_PROFILE_END_SESSION();

	EC_PROFILE_BEGIN_SESSION("Shutdown", "EchoProfile-Shutdown.json");
	app->Close();
	delete app;
	EC_PROFILE_END_SESSION();
}

#endif