#pragma once

#include "Base.h"

#ifdef ECHO_PLATFORM_WIN

extern Echo::Application* Echo::CreateApplication(HINSTANCE);

int WinMain(HINSTANCE hInst, LPSTR lpCmdLine, int nShowCmd)
{
	Echo::Log::Init();

	auto app = Echo::CreateApplication(hInst);
	app->Run();
	delete app;
}

#endif