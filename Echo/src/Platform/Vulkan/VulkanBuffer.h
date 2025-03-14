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
		VulkanVertexBuffer(Device* device, float* data, uint32_t size, bool isDynamic);
		VulkanVertexBuffer(Device* device, uint32_t size, bool isDynamic);
		virtual ~VulkanVertexBuffer();

		virtual void Bind(CommandBuffer* cmd) override;
		virtual void SetData(void* data, uint32_t size) override;
		
		virtual void* GetMappedData() override { return m_Device->GetMappedData(m_Buffer); };
	private:
		void CreateBuffer(float* data, uint32_t size, bool isDynamic);
		void CreateBuffer(uint32_t size, bool isDyamic);
	private:
		VulkanDevice* m_Device;

		AllocatedBuffer m_Buffer;
	};

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(Device* device, std::vector<uint32_t> indices);
		VulkanIndexBuffer(Device* device, uint32_t* indices, uint32_t size);
		virtual ~VulkanIndexBuffer();

		virtual void Bind(CommandBuffer* cmd) override;
	private:
		void CreateBuffer(std::vector<uint32_t> indices);
		void CreateBuffer(uint32_t* indices, uint32_t count);
	private:
		VulkanDevice* m_Device;

		AllocatedBuffer m_Buffer;
	};

	class VulkanIndirectBuffer : public IndirectBuffer
	{
	public:
		VulkanIndirectBuffer(Device* device);
		virtual ~VulkanIndirectBuffer();

		virtual void AddToIndirectBuffer(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) override;
		virtual void ClearIndirectBuffer() override;

		AllocatedBuffer GetBuffer() { return m_Buffer; }
	private:
		void CreateBuffer();
		void UpdateIndirectBufferGPU();
	private:
		VulkanDevice* m_Device;

		std::vector<VkDrawIndexedIndirectCommand> m_IndirectCommands{};
		AllocatedBuffer m_Buffer;
	};

	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(Device* device, void* data, uint32_t size);
		virtual ~VulkanUniformBuffer();

		virtual void SetData(void* data, uint32_t size) override;

		AllocatedBuffer GetBuffer() { return m_Buffer; }
		uint32_t GetSize() { return m_Size; }
	private:
		void CreateBuffer(void* data, uint32_t size);
	private:
		VulkanDevice* m_Device;
		AllocatedBuffer m_Buffer;

		uint32_t m_Size;
	};

}