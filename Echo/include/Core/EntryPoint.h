#pragma once

#include "Base.h"

#ifdef ECHO_PLATFORM_WIN

#include <windows.h>

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