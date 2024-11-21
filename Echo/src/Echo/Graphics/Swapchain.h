#pragma once

namespace Echo 
{

	struct Extent2D 
	{
		uint32_t Width;
		uint32_t Height;
	};

	class Swapchain 
	{
	public:
		virtual ~Swapchain() = default;

		virtual uint32_t AcquireNextImage() = 0;
		
		virtual void* GetSwapchainImage(uint32_t imageIndex) = 0;
		virtual void* GetSwapchainImageView(uint32_t imageIndex) = 0;

		virtual Extent2D GetExtent() = 0;
	};

}