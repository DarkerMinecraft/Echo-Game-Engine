#pragma once

#include <Core/Application.h>

#ifdef ECHO_PLATFORM_WIN

extern Echo::Application* Echo::CreateApplication();

int main()
{
	Echo::Log::Init();

	auto app = Echo::CreateApplication();
	app->Run();
	app->Close();
	delete app;
}

#endif