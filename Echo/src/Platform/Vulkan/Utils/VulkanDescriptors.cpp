#include "pch.h"
#include "VulkanDescriptors.h"

#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanDevice.h"

namespace Echo
{

	void DescriptorAllocatorGrowable::Init(VkDevice device, uint32_t initialSets, std::span<PoolSizeRatio> poolRatios)
	{
		m_Ratios.clear();

		for (auto r : poolRatios)
		{
			m_Ratios.push_back(r);
		}

		VkDescriptorPool newPool = CreatePool(device, initialSets, poolRatios);

		m_SetsPerPool = initialSets * 1.5;

		m_ReadyPools.push_back(newPool);
	}

	void DescriptorAllocatorGrowable::ClearPools(VkDevice device)
	{
		for (auto p : m_ReadyPools)
		{
			vkResetDescriptorPool(device, p, 0);
		}
		for (auto p : m_FullPools)
		{
			vkResetDescriptorPool(device, p, 0);
			m_ReadyPools.push_back(p);
		}
		m_FullPools.clear();
	}

	void DescriptorAllocatorGrowable::DestroyPools(VkDevice device)
	{
		for (auto p : m_ReadyPools)
		{
			vkDestroyDescriptorPool(device, p, nullptr);
		}
		m_ReadyPools.clear();

		for (auto p : m_FullPools)
		{
			vkDestroyDescriptorPool(device, p, nullptr);
		}
		m_FullPools.clear();
	}

	VkDescriptorSet DescriptorAllocatorGrowable::Allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext /*= nullptr*/)
	{
		VkDescriptorPool poolToUse = GetPool(device);

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = pNext;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = poolToUse;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet ds;
		VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &ds);

		if (result != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to allocate descriptor set!");
		}

		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
		{

			m_FullPools.push_back(poolToUse);

			poolToUse = GetPool(device);
			allocInfo.descriptorPool = poolToUse;

			vkAllocateDescriptorSets(device, &allocInfo, &ds);
		}

		m_ReadyPools.push_back(poolToUse);
		return ds;
	}

	VkDescriptorPool DescriptorAllocatorGrowable::GetPool(VkDevice device)
	{
		VkDescriptorPool newPool;
		if (m_ReadyPools.size() != 0) 
		{
			newPool = m_ReadyPools.back();
			m_ReadyPools.pop_back();
		}
		else 
		{
			newPool = CreatePool(device, m_SetsPerPool, m_Ratios);
			
			m_SetsPerPool *= 1.5;
			if (m_SetsPerPool > 4092) 
			{
				m_SetsPerPool = 4092;
			}
		}

		return newPool;
	}

	VkDescriptorPool DescriptorAllocatorGrowable::CreatePool(VkDevice device, uint32_t setCount, std::span<PoolSizeRatio> poolRatios)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (PoolSizeRatio ratio : poolRatios)
		{
			poolSizes.push_back(VkDescriptorPoolSize{
				.type = ratio.Type,
				.descriptorCount = uint32_t(ratio.Ratio * setCount)
								});
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = 0;
		poolInfo.maxSets = setCount;
		poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
		poolInfo.pPoolSizes = poolSizes.data();

		VkDescriptorPool newPool;
		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &newPool) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create descriptor pool!");
		}

		return newPool;
	}

	void DescriptorWriter::WriteImage(int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type)
	{
		VkDescriptorImageInfo& info = ImageInfos.emplace_back(VkDescriptorImageInfo{
		.sampler = sampler,
		.imageView = image,
		.imageLayout = layout
															  });

		VkWriteDescriptorSet write = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };

		write.dstBinding = binding;
		write.dstSet = VK_NULL_HANDLE; 
		write.descriptorCount = 1;
		write.descriptorType = type;
		write.pImageInfo = &info;

		Writes.push_back(write);
	}

	void DescriptorWriter::WriteImage(int index, int binding, VkImageView image, VkSampler sampler, VkImageLayout layout, VkDescriptorType type)
	{
		VkDescriptorImageInfo& info = ImageInfos.emplace_back(VkDescriptorImageInfo{
		.sampler = sampler,
		.imageView = image,
		.imageLayout = layout
															  });

		VkWriteDescriptorSet write = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
		write.dstBinding = binding;
		write.dstArrayElement = index;  
		write.dstSet = VK_NULL_HANDLE;
		write.descriptorCount = 1;
		write.descriptorType = type;
		write.pImageInfo = &info;

		Writes.push_back(write);
	}

	void DescriptorWriter::WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type)
	{
		VkDescriptorBufferInfo& info = BufferInfos.emplace_back(VkDescriptorBufferInfo{
		.buffer = buffer,
		.offset = offset,
		.range = size
																});

		VkWriteDescriptorSet write = { .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };

		write.dstBinding = binding;
		write.dstSet = VK_NULL_HANDLE; 
		write.descriptorCount = 1;
		write.descriptorType = type;
		write.pBufferInfo = &info;

		Writes.push_back(write);
	}

	void DescriptorWriter::Clear()
	{
		ImageInfos.clear();
		Writes.clear();
		BufferInfos.clear();
	}

	void DescriptorWriter::UpdateSet(VkDevice device, VkDescriptorSet set)
	{
		for (VkWriteDescriptorSet& write : Writes)
		{
			write.dstSet = set;
		}

		vkUpdateDescriptorSets(device, (uint32_t)Writes.size(), Writes.data(), 0, nullptr);
	}

	void DescriptorLayoutBuilder::AddBinding(uint32_t binding, uint32_t count, VkShaderStageFlags shaderStages, VkDescriptorType type)
	{
		VkDescriptorSetLayoutBinding newbind{};
		newbind.binding = binding;
		newbind.descriptorCount = count;
		newbind.descriptorType = type;
		newbind.stageFlags = shaderStages;

		Bindings.push_back(newbind);
	}

	void DescriptorLayoutBuilder::Clear()
	{
		Bindings.clear();
	}

	VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, void* pNext /*= nullptr*/, VkDescriptorSetLayoutCreateFlags flags /*= 0*/)
	{
		VkDescriptorSetLayoutCreateInfo info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		info.pNext = pNext;

		info.pBindings = Bindings.data();
		info.bindingCount = (uint32_t)Bindings.size();
		info.flags = flags;

		VkDescriptorSetLayout set;
		if (vkCreateDescriptorSetLayout(device, &info, nullptr, &set) != VK_SUCCESS) 
		{
			EC_CORE_CRITICAL("Failed to create descriptor set layout!");
		}

		return set;
	}

}