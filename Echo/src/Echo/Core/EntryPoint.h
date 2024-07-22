#pragma once

#include "Base.h"

#ifdef ECHO_PLATFORM_WIN

extern Echo::Application* Echo::CreateApplication();

int main(int argc, char** argv) 
{
	Echo::Log::Init();

	auto app = Echo::CreateApplication();
	app->Run();
	delete app;
}

#endif