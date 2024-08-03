#pragma once

#include "Base.h"
#include "pch.h"

namespace Echo 
{
	struct WindowProps 
	{
		const std::string& Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string title = "Echo Engine", unsigned int width = 1280, unsigned int height = 720)
			: Title(title), Width(width), Height(height) {}
	};

	class Window 
	{
	public:
		static Window* Create(const WindowProps& props = WindowProps(), HINSTANCE hInst = nullptr);
		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;
	};
}