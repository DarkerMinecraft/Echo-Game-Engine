#pragma once

#include "Echo/Graphics/CommandList.h"

#include "VulkanDevice.h"
#include "VulkanModel.h"

#include <map>

namespace Echo 
{
	class VulkanVertexBuffer : public VertexBuffer 
	{
	public:
		VulkanVertexBuffer(std::vector<Vertex> vertices);
		virtual ~VulkanVertexBuffer();

		virtual void Bind() override;
	private:
		void CreateVertexBuffer(std::vector<Vertex> vertices);
	private:
		VulkanDevice* m_Device;

		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
	};

	class VulkanCommandBuffer : public CommandBuffer 
	{
	public:
		VulkanCommandBuffer(VulkanDevice* device, VkCommandPool commandPool);
		virtual ~VulkanCommandBuffer();

		virtual void AddMesh(Ref<Resource> resource, Ref<Model> model) override;

		virtual void Begin() override;
		virtual void Submit() override;
		virtual void End() override;
	public:
		VkCommandBuffer GetBuffer() { return m_CommandBuffer; }
	private:
		void CreateCommandBuffer();
	private:
		VulkanDevice* m_Device;
		VkCommandPool m_CommandPool;

		VkCommandBuffer m_CommandBuffer;

		uint32_t m_ImageCount;

		std::map<Ref<Resource>, std::vector<VulkanModel*>> m_Meshes;
	};
}