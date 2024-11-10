#include "pch.h"
#include "ImGuiVulkan.h"

#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_glfw.h"

#include "Echo/Core/Application.h"
#include "Echo/Graphics/RenderCommand.h"

namespace Echo 
{

	void ImGuiVulkan::Start()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
	}

	void ImGuiVulkan::Attach()
	{
		GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());

		ImGui_ImplVulkan_InitInfo initInfo{};
		RenderCommand::SetDefaultImGui(initInfo);

		ImGui_ImplGlfw_InitForVulkan(window, true);
		ImGui_ImplVulkan_Init(&initInfo);
	}

	void ImGuiVulkan::Render(ImGuiIO& io)
	{
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), RenderCommand::GetCurrentCommandBuffer());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void ImGuiVulkan::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
	}

}