#include "pch.h"
#include "ImGuiLayer.h"

#include "Echo/Core/Application.h"

#include "Echo/Graphics/RenderCommand.h"

#include "imgui.h"

namespace Echo 
{

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		m_ImGuiVulkan = new ImGuiVulkan();
	}

	ImGuiLayer::~ImGuiLayer()
	{

	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		m_ImGuiVulkan->Attach();
	}

	void ImGuiLayer::OnDetach()
	{
		m_ImGuiVulkan->Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		static bool show = true;
		ImGui::ShowDemoWindow(&show);
	}

	void ImGuiLayer::Begin()
	{
		m_ImGuiVulkan->Start();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();
		Window& window = Application::Get().GetWindow();
		io.DisplaySize = ImVec2(window.GetWidth(), window.GetHeight());

		ImGui::Render();
		m_ImGuiVulkan->Render(io);
	}

}