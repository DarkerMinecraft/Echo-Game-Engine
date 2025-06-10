#pragma once

#include "Graphics/Primitives/CommandBuffer.h"
#include "Graphics/Commands/ICommand.h"

#include "Graphics/Primitives/Buffer.h"

namespace Echo 
{

	class VulkanBindVertexBufferCommand : public ICommand
	{
	public:
		VulkanBindVertexBufferCommand(Ref<VertexBuffer> vertexBuffer)
			: m_VertexBuffer(vertexBuffer)
		{}
		virtual void Execute(CommandBuffer* cmd) override { m_VertexBuffer->Bind(cmd); };
	private:
		Ref<VertexBuffer> m_VertexBuffer;
	};

	class VulkanBindIndicesBufferCommand : public ICommand 
	{
	public:
		VulkanBindIndicesBufferCommand(Ref<IndexBuffer> indexBuffer)
			: m_IndexBuffer(indexBuffer)
		{}
		virtual void Execute(CommandBuffer* cmd) override { m_IndexBuffer->Bind(cmd); };
	private:
		Ref<IndexBuffer> m_IndexBuffer;
	};

}