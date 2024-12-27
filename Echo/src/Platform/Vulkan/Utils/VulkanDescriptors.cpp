#include "pch.h"
#include "VulkanDescriptors.h"

#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanDevice.h"

namespace Echo
{

	void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type)
	{
		VkDescriptorSetLayoutBinding newbind{};
		newbind.binding = binding;
		newbind.descriptorCount = 1;
		newbind.descriptorType = type;

		Bindings.push_back(newbind);
	}

	void DescriptorLayoutBuilder::Clear()
	{
		Bindings.clear();
	}

	VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext /*= nullptr*/, VkDescriptorSetLayoutCreateFlags flags /*= 0*/)
	{
		for (auto& b : Bindings)
		{
			b.stageFlags |= shaderStages;
		}

		VkDescriptorSetLayoutCreateInfo info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		info.pNext = pNext;

		info.pBindings = Bindings.data();
		info.bindingCount = (uint32_t)Bindings.size();
		info.flags = flags;

		VkDescriptorSetLayout set;
		if (vkCreateDescriptorSetLayout(device, &info, nullptr, &set) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create descriptor set layout");
		}

		return set;
	}

	void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (PoolSizeRatio ratio : poolRatios)
		{
			poolSizes.push_back(VkDescriptorPoolSize{
				.type = ratio.Type,
				.descriptorCount = uint32_t(ratio.Ratio * maxSets)
			});
		}

		VkDescriptorPoolCreateInfo pool_info = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };
		pool_info.flags = 0;
		pool_info.maxSets = maxSets;
		pool_info.poolSizeCount = (uint32_t)poolSizes.size();
		pool_info.pPoolSizes = poolSizes.data();

		vkCreateDescriptorPool(device, &pool_info, nullptr, &Pool);
	}

	void DescriptorAllocator::ClearDescriptors(VkDevice device)
	{
		vkResetDescriptorPool(device, Pool, 0);
	}

	void DescriptorAllocator::DestroyPool(VkDevice device)
	{
		vkDestroyDescriptorPool(device, Pool, nullptr);
	}

	VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
	{
		VkDescriptorSetAllocateInfo allocInfo = { .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		allocInfo.pNext = nullptr;
		allocInfo.descriptorPool = Pool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &layout;

		VkDescriptorSet ds;
		if (vkAllocateDescriptorSets(device, &allocInfo, &ds) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to allocate descriptor sets");
		}

		return ds;
	}

	/*VulkanDescriptorAllocatorGrowable::VulkanDescriptorAllocatorGrowable(VulkanDevice* device)
		: m_Device(device)
	{

	}

	void VulkanDescriptorAllocatorGrowable::Init(uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
	{
		m_Ratios.clear();

		for (auto r : poolRatios)
		{
			m_Ratios.push_back(r);
		}

		VkDescriptorPool newPool = CreatePool(maxSets, poolRatios);

		m_SetsPerPool = maxSets * 1.5;

		m_ReadyPools.push_back(newPool);
	}

	void VulkanDescriptorAllocatorGrowable::ClearPools()
	{
		for (auto p : m_ReadyPools)
		{
			vkResetDescriptorPool(m_Device->GetDevice(), p, 0);
		}
		for (auto p : m_FullPools)
		{
			vkResetDescriptorPool(m_Device->GetDevice(), p, 0);
			m_ReadyPools.push_back(p);
		}
		m_FullPools.clear();
	}

	void VulkanDescriptorAllocatorGrowable::DestroyPools()
	{
		for (auto p : m_ReadyPools)
		{
			vkDestroyDescriptorPool(m_Device->GetDevice(), p, nullptr);
		}
		m_ReadyPools.clear();
		for (auto p : m_FullPools)
		{
			vkDestroyDescriptorPool(m_Device->GetDevice(), p, nullptr);
		}
		m_FullPools.clear();
	}

	void* VulkanDescriptorAllocatorGrowable::Allocate(void* layout, void* pNext)
	{
		VkDescriptorPool poolToUse = GetPool();

		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.pNext = pNext;
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = poolToUse;
		allocInfo.descriptorSetCount = 1;

		VkDescriptorSetLayout sLayout = (VkDescriptorSetLayout)layout;
		allocInfo.pSetLayouts = &sLayout;

		VkDescriptorSet ds;
		VkResult result = vkAllocateDescriptorSets(m_Device->GetDevice(), &allocInfo, &ds);

		if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
		{

			m_FullPools.push_back(poolToUse);

			poolToUse = GetPool();
			allocInfo.descriptorPool = poolToUse;

			if (vkAllocateDescriptorSets(m_Device->GetDevice(), &allocInfo, &ds) != VK_SUCCESS)
			{
				throw std::runtime_error("Failed to allocate descriptor sets");
			}
		}

		m_ReadyPools.push_back(poolToUse);
		return ds;
	}

	VkDescriptorPool VulkanDescriptorAllocatorGrowable::GetPool()
	{
		VkDescriptorPool newPool;
		if (m_ReadyPools.size() != 0)
		{
			newPool = m_ReadyPools.back();
			m_ReadyPools.pop_back();
		}
		else
		{
			newPool = CreatePool(m_SetsPerPool, m_Ratios);

			m_SetsPerPool = m_SetsPerPool * 1.5;
			if (m_SetsPerPool > 4092)
			{
				m_SetsPerPool = 4092;
			}
		}

		return newPool;
	}

	VkDescriptorPool VulkanDescriptorAllocatorGrowable::CreatePool(uint32_t setCount, std::span<PoolSizeRatio> poolRatios)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;
		for (PoolSizeRatio ratio : poolRatios)
		{
			poolSizes.push_back(VkDescriptorPoolSize{
				.type = ToVulkanDescriptorType(ratio.Type),
				.descriptorCount = uint32_t(ratio.Ratio * setCount)
								});
		}

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags = 0;
		pool_info.maxSets = setCount;
		pool_info.poolSizeCount = (uint32_t)poolSizes.size();
		pool_info.pPoolSizes = poolSizes.data();

		VkDescriptorPool newPool;
		vkCreateDescriptorPool(m_Device->GetDevice(), &pool_info, nullptr, &newPool);
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
	}*/

}