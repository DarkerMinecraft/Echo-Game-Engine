#pragma once

#include "Resource.h"

namespace Echo 
{
	enum class ImageFormat;
	enum class PresentMode 
	{
		Immediate, 
		Vsync, 
		Mailbox
	};

	struct Extent2D 
	{
		uint32_t Width;
		uint32_t Height;
	};

	struct SwapchainWin32CreateInfo 
	{
		void* Hwnd;
		ImageFormat PreferredFormat;
		uint32_t ImageCount;
		PresentMode PresentMode;
	};

	struct SwapchainResizeInfo {};

	class Swapchain 
	{
	public:
		Swapchain() = default;
		virtual ~Swapchain() = default;

		virtual Extent2D GetExtent() = 0; 

		virtual uint32_t AcquireNextImage() = 0;

		virtual void StartRenderPass(uint32_t imageCount) = 0;
		virtual void SetState() = 0;
		virtual void EndRenderPass() = 0;
	};
}