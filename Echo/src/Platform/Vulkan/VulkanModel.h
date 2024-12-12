#pragma once

#include "Echo/Graphics/Model.h"

#include "VulkanDevice.h"
#include <Echo/Graphics/Texture.h>

namespace Echo 
{

	class VulkanModel : public Model 
	{
	public:
		VulkanModel(std::vector<uint32_t> indices, std::vector<Vertex> vertices, Ref<Texture> texture);
		VulkanModel(const std::string& objFilePath, Ref<Texture> texture);
		virtual ~VulkanModel();

		virtual void Bind(Pipeline* pipeline) override; 
		virtual void UpdatePushConstants(void* pushConstants) override { m_PushConstants = pushConstants; };

		virtual uint32_t GetIndicesCount() override { return m_IndicesCount; }
	public:
		static VkVertexInputBindingDescription GetBindingDescription();
		static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
	private:
		void CreateModel(std::vector<uint32_t> indices, std::vector<Vertex> vertices);
		void CreateModel(const std::string& objFilePath);
	private:
		VulkanDevice* m_Device;
		uint32_t m_IndicesCount;

		void* m_PushConstants;

		Ref<Texture> m_Texture;

		GPUMeshBuffers m_MeshBuffers;
	};

}