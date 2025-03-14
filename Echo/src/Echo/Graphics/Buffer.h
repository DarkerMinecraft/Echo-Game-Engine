#pragma once

#include "Echo/Core/Base.h"

#include "Echo/Graphics/CommandBuffer.h"

#include <vector>

namespace Echo 
{

	class VertexBuffer 
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind(CommandBuffer* cmd) = 0;
		virtual void* GetMappedData() = 0; 

		virtual void SetData(void* data, uint32_t size) = 0;

		static Ref<VertexBuffer> Create(float* data, uint32_t size, bool isDynamic = false);
		static Ref<VertexBuffer> Create(uint32_t size, bool isDynamic = false);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind(CommandBuffer* cmd) = 0;

		static Ref<IndexBuffer> Create(std::vector<uint32_t> indices);
		static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);
	};

	class IndirectBuffer 
	{
	public:
		virtual ~IndirectBuffer() = default;

		virtual void AddToIndirectBuffer(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance) = 0;
		virtual void ClearIndirectBuffer() = 0;

		static Ref<IndirectBuffer> Create();
	};

	class UniformBuffer 
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(void* data, uint32_t size) = 0;

		static Ref<UniformBuffer> Create(void* data, uint32_t size);
	};

}