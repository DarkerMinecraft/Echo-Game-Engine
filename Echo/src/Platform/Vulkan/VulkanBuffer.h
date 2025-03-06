#pragma once

#include "Echo/Graphics/Buffer.h"

#include "VulkanDevice.h"
#include "Utils/VulkanTypes.h"

#include <vector>

namespace Echo 
{

	class VulkanVertexBuffer : public VertexBuffer 
	{
	public:
		VulkanVertexBuffer(Device* device, std::vector<float> vertices);
		VulkanVertexBuffer(Device* device, std::vector<float> vertices, std::vector<float> colors);
		virtual ~VulkanVertexBuffer();

		virtual void Bind(CommandBuffer* cmd) override;
	private:
		void CreateBuffer(std::vector<float> vertices);
		void CreateBuffer(std::vector<float> vertices, std::vector<float> colors);
	private:
		VulkanDevice* m_Device;

		AllocatedBuffer m_Buffer;
	};

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(Device* device, std::vector<uint32_t> indices);
		virtual ~VulkanIndexBuffer();

		virtual void Bind(CommandBuffer* cmd) override;
	private:
		void CreateBuffer(std::vector<uint32_t> indices);
	private:
		VulkanDevice* m_Device;

		AllocatedBuffer m_Buffer;
	};

}