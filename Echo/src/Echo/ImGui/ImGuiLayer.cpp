#include "pch.h"
#include "ImGuiLayer.h"

#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanDevice.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_glfw.h>
#include <GLFW/glfw3.h>
#include <Echo/Graphics/CommandList.h>
#include "../Graphics/Commands/CommandFactory.h"

namespace Echo
{

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{

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

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		ImGui_ImplGlfw_InitForVulkan(window, true);

		VulkanDevice* device = static_cast<VulkanDevice*>(app.GetWindow().GetDevice());

		ImGui_ImplVulkan_InitInfo initInfo{};

		initInfo.Instance = device->GetInstance();
		initInfo.PhysicalDevice = device->GetPhysicalDevice();
		initInfo.Device = device->GetDevice();
		initInfo.Queue = device->GetGraphicsQueue();
		initInfo.DescriptorPool = device->GetImGuiDescriptorPool();
		initInfo.MinImageCount = 3;
		initInfo.ImageCount = 3;
		initInfo.UseDynamicRendering = true;

		initInfo.PipelineRenderingCreateInfo = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO };
		initInfo.PipelineRenderingCreateInfo.colorAttachmentCount = 1;

		static VkFormat format = VK_FORMAT_B8G8R8A8_UNORM;
		initInfo.PipelineRenderingCreateInfo.pColorAttachmentFormats = &format;

		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&initInfo);
		ImGui_ImplVulkan_CreateFontsTexture();
		
		m_DrawImage = Image::Create({ .DrawImage = true });
	}

	void ImGuiLayer::OnDetach()
	{
		m_ImGuiImage->Destroy();
	}

	void ImGuiLayer::OnUpdate(Timestep ts)
	{
		
	}

	void ImGuiLayer::OnImGuiRender()
	{
		
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplGlfw_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		DrawImGui();
	}

	void ImGuiLayer::DrawImGui()
	{
		Application& app = Application::Get();
		VulkanDevice* device = static_cast<VulkanDevice*>(app.GetWindow().GetDevice());
		m_ImGuiImage = Image::Create({ .DrawImageExtent = true, .Format = BGRA8 });

		CommandList cmd;

		cmd.Begin();
		cmd.RecordCommand(CommandFactory::TransitionImageCommand(m_DrawImage, General, TransferSrc));
		cmd.RecordCommand(CommandFactory::TransitionImageCommand(m_ImGuiImage, Undefined, TransferDst));
		cmd.RecordCommand(CommandFactory::CopyImageToImageCommand(m_DrawImage, m_ImGuiImage));
		cmd.RecordCommand(CommandFactory::TransitionImageCommand(m_ImGuiImage, TransferDst, ColorAttachment));
		cmd.RecordCommand(CommandFactory::BeginRenderingCommand(m_ImGuiImage));
		cmd.RecordCommand(CommandFactory::RenderImGuiCommand());
		cmd.RecordCommand(CommandFactory::EndRenderingCommand());
		cmd.RecordCommand(CommandFactory::TransitionImageCommand(m_ImGuiImage, ColorAttachment, General));
		cmd.SetSrcImage(m_ImGuiImage);
		cmd.Execute();

		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	void ImGuiLayer::Destroy()
	{
		Application& app = Application::Get();
		VulkanDevice* device = static_cast<VulkanDevice*>(app.GetWindow().GetDevice());

		vkDeviceWaitIdle(device->GetDevice());
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplVulkan_Shutdown();
		ImGui::DestroyContext();
	}

}