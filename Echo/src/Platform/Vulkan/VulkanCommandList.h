#pragma once

#include "Echo/Graphics/CommandList.h"

#include "VulkanDevice.h"
#include <map>

namespace Echo 
{
	class VulkanCommandBuffer : public CommandBuffer 
	{
	public:
		VulkanCommandBuffer(VulkanDevice* device, VkCommandPool commandPool);
		virtual ~VulkanCommandBuffer();

		virtual void AddMesh(Ref<Resource> resource, Vertex vertex) override;

		virtual void Begin() override;
		virtual void Draw() override;
		virtual void End() override;
	public:
		VkCommandBuffer GetBuffer() { return m_CommandBuffer; }
	private:
		void CreateCommandBuffer();
	private:
		VulkanDevice* m_Device;
		VkCommandPool m_CommandPool;

		VkCommandBuffer m_CommandBuffer;

		std::map<Ref<Resource>, std::vector<Vertex>> m_Meshes;
	};
}