#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <span>
#include <deque>

namespace Echo 
{

	struct DescriptorLayoutBuilder
	{

		std::vector<VkDescriptorSetLayoutBinding> Bindings;

		void AddBinding(uint32_t binding, uint32_t count, VkShaderStageFlags shaderStages, VkDescriptorType type);
		void Clear();
		VkDescriptorSetLayout Build(VkDevice device, void* pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0);
	};
	
	struct DescriptorAllocatorGrowable
	{
	public:
		struct PoolSizeRatio
		{
			VkDescriptorType Type;
			float Ratio;
		};

		void Init(VkDevice device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios);
		void ClearPools(VkDevice device);
		void DestroyPools(VkDevice device);

		VkDescriptorSet Allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext = nullptr);
	private:
		VkDescriptorPool GetPool(VkDevice device);
		VkDescriptorPool CreatePool(VkDevice device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios);

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
		void WriteImage(int index, int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type);
		void WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type);

		void Clear();
		void UpdateSet(VkDevice device, VkDescriptorSet set);
	};
}