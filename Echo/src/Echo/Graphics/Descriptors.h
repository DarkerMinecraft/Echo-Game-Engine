#pragma once

#include <span>

namespace Echo 
{

	enum class DescriptorType 
	{
		UniformBuffer,        // Vulkan: VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
		Sampler,              // Vulkan: VK_DESCRIPTOR_TYPE_SAMPLER
		CombinedImageSampler, // Vulkan: VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
		SampledImage,         // Vulkan: VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE
		StorageImage,         // Vulkan: VK_DESCRIPTOR_TYPE_STORAGE_IMAGE
		StorageBuffer,        // Vulkan: VK_DESCRIPTOR_TYPE_STORAGE_BUFFER
		InputAttachment,      // Vulkan: VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
		ShaderResourceView,   // DirectX: SRV
		UnorderedAccessView,
		Unknown
	};

	class DescriptorAllocatorGrowable 
	{
	public:
		struct PoolSizeRatio
		{
			DescriptorType Type;
			float Ratio;
		};

		virtual ~DescriptorAllocatorGrowable() = default;

		virtual void Init(uint32_t initialSets, std::span<PoolSizeRatio> poolRatios) = 0;
		virtual void ClearPools() = 0;
		virtual void DestroyPools() = 0;

		virtual void* Allocate(void* layout, void* pNext = nullptr) = 0;
	};
}