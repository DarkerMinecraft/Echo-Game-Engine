#include "pch.h"
#include "VulkanGraphicsContext.h"

#include "Echo/Core/Application.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Setup/VulkanSurface.h"

namespace Echo
{
	VulkanGraphicsContext::VulkanGraphicsContext(GLFWwindow* window)
		: m_Window(window)
	{
		m_Instance = new VulkanInstance();
		m_Surface = new VulkanSurface(m_Instance->GetInstance(), window);
		m_Device = new VulkanDevice(m_Instance->GetInstance(), m_Surface->GetSurface());
		m_SwapChain = new VulkanSwapChain(m_Instance->GetInstance(), m_Surface->GetSurface(), window, m_Device);
	}

	void VulkanGraphicsContext::Init()
	{
		m_Instance->SetupDebugMessenger();
		m_Device->PickPhysicalDevice();
		m_Device->CreateLogicalDevice();
		m_SwapChain->CreateSwapChain();
		m_SwapChain->CreateImageViews();
	}

	void VulkanGraphicsContext::CleanUp()
	{
		m_SwapChain->CleanUp();
		m_Device->CleanUp();
		m_Surface->CleanUp();
		m_Instance->CleanUp();
	}

}
