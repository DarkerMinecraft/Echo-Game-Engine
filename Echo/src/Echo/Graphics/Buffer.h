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

		static Ref<VertexBuffer> Create(std::vector<float> vertices);
		static Ref<VertexBuffer> Create(std::vector<float> vertices, std::vector<float> color);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind(CommandBuffer* cmd) = 0;

		static Ref<IndexBuffer> Create(std::vector<uint32_t> indices);
	};

}