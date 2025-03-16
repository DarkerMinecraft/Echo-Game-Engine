#pragma once

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

namespace Echo 
{

	struct AllocatedImage
	{
		VkImage Image;
		VkImageView ImageView;
		VmaAllocation Allocation;
		VkExtent3D ImageExtent;
		VkFormat ImageFormat;
		VkImageLayout ImageLayout;

		VkSampleCountFlagBits Samples;
		VkSampler Sampler;
		bool DepthTexture = false;
		bool Destroyed = false;
	};

	struct AllocatedBuffer
	{
		VkBuffer Buffer;
		VmaAllocation Allocation;
		VmaAllocationInfo Info;
	};

}