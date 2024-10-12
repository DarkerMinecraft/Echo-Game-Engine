#pragma once

#include "Echo/Graphics/GraphicsContext.h"
#include "Setup/VulkanDevice.h"
#include "Setup/VulkanInstance.h"
#include "Setup/VulkanSwapChain.h"

struct GLFWwindow;

namespace Echo
{
	class VulkanSurface;

	class VulkanGraphicsContext : public GraphicsContext
	{
	public:
		VulkanGraphicsContext(GLFWwindow* window);

		virtual void Init() override;
		virtual void SwapBuffers() override {}
		virtual void CleanUp() override;
		
		VulkanDevice* GetVulkanDevice() { return m_Device; }
		VulkanInstance* GetVulkanInstance() { return m_Instance; }
		VulkanSwapChain* GetVulkanSwapChain() { return m_SwapChain; }
	private:
		GLFWwindow* m_Window;

		VulkanInstance* m_Instance;
		VulkanDevice* m_Device;
		VulkanSurface* m_Surface;
		VulkanSwapChain* m_SwapChain;
	};

}
