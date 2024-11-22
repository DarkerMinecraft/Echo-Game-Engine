#pragma once

#include "Echo/Graphics/Model.h"

#include "VulkanDevice.h"

namespace Echo 
{

	class VulkanModel : public Model 
	{
	public:
		VulkanModel(std::vector<uint32_t> indices, std::vector<Vertex> vertices);
		virtual ~VulkanModel();

		virtual void Bind() override; 

		virtual uint32_t GetIndicesCount() override { return m_IndicesCount; }
	public:
		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
	private:
		void CreateModel(std::vector<uint32_t> indices, std::vector<Vertex> vertices);
	private:
		VulkanDevice* m_Device;
		uint32_t m_IndicesCount;

		GPUMeshBuffers m_MeshBuffers;
	};

}