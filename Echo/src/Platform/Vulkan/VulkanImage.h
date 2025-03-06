#pragma once

#include "Echo/Graphics/Image.h"
#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanImage : public Image 
	{
	public:
		VulkanImage(Device* device, const ImageDescription& desc);
		~VulkanImage();

		virtual uint32_t GetWidth() override;
		virtual uint32_t GetHeight() override;

		virtual void* GetImageHandle() override;

		virtual void Destroy() override { m_Device->DestroyImage(m_Image); };

		AllocatedImage GetImage();
	private:
		void CreateAllocatedImage(const ImageDescription& desc);
	private:
		VulkanDevice* m_Device;
		uint32_t m_Width, m_Height;

		AllocatedImage m_Image;

		bool m_UseDrawImage;
		bool m_DrawImageExtent;
	};

}