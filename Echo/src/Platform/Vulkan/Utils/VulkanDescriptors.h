#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <span>
#include <deque>

namespace Echo 
{
	
	/*static VkDescriptorType ToVulkanDescriptorType(DescriptorType type)
	{
		switch (type)
		{
			case DescriptorType::UniformBuffer:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case DescriptorType::Sampler:
				return VK_DESCRIPTOR_TYPE_SAMPLER;
			case DescriptorType::CombinedImageSampler:
				return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			case DescriptorType::SampledImage:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case DescriptorType::StorageImage:
				return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
			case DescriptorType::StorageBuffer:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case DescriptorType::InputAttachment:
				return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			case DescriptorType::ShaderResourceView:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; // Closest Vulkan equivalent
			case DescriptorType::UnorderedAccessView:
				return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE; // Closest Vulkan equivalent
			default:
				throw std::runtime_error("Unsupported DescriptorType");
		}
	} */

	struct DescriptorLayoutBuilder
	{
		std::vector<VkDescriptorSetLayoutBinding> Bindings;

		void AddBinding(uint32_t binding, VkDescriptorType type);
		void Clear();

		VkDescriptorSetLayout Build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
	};

	struct DescriptorAllocator
	{
		struct PoolSizeRatio
		{
			VkDescriptorType Type;
			float Ratio;
		};

		VkDescriptorPool Pool;

		void InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios);
		void ClearDescriptors(VkDevice device);
		void DestroyPool(VkDevice device);

		VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout);
	};

	/*class VulkanDescriptorAllocatorGrowable : public DescriptorAllocatorGrowable
	{
	public:
		VulkanDescriptorAllocatorGrowable(VulkanDevice* device);

		virtual void Init(uint32_t initialSets, std::span<PoolSizeRatio> poolRatios) override;
		virtual void ClearPools() override;
		virtual void DestroyPools() override;

		virtual void* Allocate(void* layout, void* pNext = nullptr) override;
	private:
		VkDescriptorPool GetPool();
		VkDescriptorPool CreatePool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios);
	private:
		VulkanDevice* m_Device;

		std::vector<PoolSizeRatio> m_Ratios;
		std::vector<VkDescriptorPool> m_FullPools;
		std::vector<VkDescriptorPool> m_ReadyPools;
		uint32_t m_SetsPerPool;
	};

	struct DescriptorWriter
	{
		std::deque<VkDescriptorImageInfo> ImageInfos;
		std::deque<VkDescriptorBufferInfo> BufferInfos;
		std::vector<VkWriteDescriptorSet> Writes;

		void WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
		void WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);

		void Clear();
		void UpdateSet(VkDevice device, VkDescriptorSet set);
	};*/
}