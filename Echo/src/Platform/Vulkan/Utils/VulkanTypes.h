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
	};

	struct AllocatedBuffer
	{
		VkBuffer Buffer;
		VmaAllocation Allocation;
		VmaAllocationInfo Info;
	};

	struct GPUMeshBuffers
	{
		AllocatedBuffer IndexBuffer;
		AllocatedBuffer VertexBuffer;	
	};

	struct GPUDrawPushConstants
	{
		glm::mat4 WorldMatrix;
	};

}