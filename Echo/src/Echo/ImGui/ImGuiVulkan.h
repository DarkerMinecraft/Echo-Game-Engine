#pragma once

#include "imgui.h"

namespace Echo 
{
	class ImGuiVulkan 
	{
	public:
		ImGuiVulkan() = default;

		void Attach();
		void Start();
		void Render(ImGuiIO& io);
		void Shutdown();
	};
}