#pragma once

#include "Resource.h"

namespace Echo 
{
	class CommandBuffer 
	{
	public:
		virtual ~CommandBuffer() = default;

		virtual void Begin(uint32_t imageCount) = 0;
		virtual void Draw(uint32_t imageCount, Vertex vertex) = 0;
		virtual void End(uint32_t imageCount) = 0;
	};
}