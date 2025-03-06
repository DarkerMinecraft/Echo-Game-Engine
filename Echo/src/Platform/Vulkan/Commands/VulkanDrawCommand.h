#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "Echo/Graphics/Commands/ICommand.h"

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
}