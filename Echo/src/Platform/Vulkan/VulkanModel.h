#pragma once

#include "Echo/Graphics/GraphicsModel.h"
#include "Interface/VulkanDevice.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace Echo 
{
	class VulkanModel : public GraphicsModel
	{
	public:
		struct VulkanVertex : public Vertex 
		{
			static std::vector<VkVertexInputBindingDescription> GetBindingDescription();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescription();
		};

		VulkanModel(const std::vector<Vertex>& vertices);
		~VulkanModel() = default;

		virtual void Destroy() override;

		void Bind(VkCommandBuffer commandBuffer);
		void Draw(VkCommandBuffer commandBuffer);
	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
	private:
		VulkanDevice* m_Device;

		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		uint32_t m_VertexCount;
	};
}