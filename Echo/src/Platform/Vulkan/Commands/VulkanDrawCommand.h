#pragma once

#include "Graphics/Primitives/CommandBuffer.h"
#include "Graphics/Commands/ICommand.h"

#include "Graphics/Primitives/Buffer.h"

namespace Echo 
{

	class VulkanDrawCommand : public ICommand
	{
	public:
		VulkanDrawCommand(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
			: m_VertexCount(vertexCount), m_InstanceCount(instanceCount), m_FirstVertex(firstVertex), m_FirstInstance(firstInstance)
		{}
		virtual void Execute(CommandBuffer* cmd) override;
	private:
		uint32_t m_VertexCount;
		uint32_t m_InstanceCount;
		uint32_t m_FirstVertex;
		uint32_t m_FirstInstance;
	};

	class VulkanDrawIndexedCommand : public ICommand
	{
	public:
		VulkanDrawIndexedCommand(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
			: m_IndexCount(indexCount), m_InstanceCount(instanceCount), m_FirstIndex(firstIndex), m_VertexOffset(vertexOffset), m_FirstInstance(firstInstance)
		{}
		virtual void Execute(CommandBuffer* cmd) override;
	private:
		uint32_t m_IndexCount;
		uint32_t m_InstanceCount;
		uint32_t m_FirstIndex;
		uint32_t m_VertexOffset;
		uint32_t m_FirstInstance;
	};

	class VulkanDrawIndexedIndirect : public ICommand 
	{
	public:
		VulkanDrawIndexedIndirect(Ref<IndirectBuffer> buffer, uint32_t offset, uint32_t drawCount, uint32_t stride)
			: m_Buffer(buffer), m_Offset(offset), m_DrawCount(drawCount), m_Stride(stride)
		{}
		virtual void Execute(CommandBuffer* cmd) override;
	private:
		Ref<IndirectBuffer> m_Buffer;
		uint32_t m_Offset;
		uint32_t m_DrawCount;
		uint32_t m_Stride;
	};
}