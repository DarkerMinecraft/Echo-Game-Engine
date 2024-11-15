#pragma once

#include "Resource.h"

namespace Echo 
{
	class CommandBuffer 
	{
	public:
		virtual ~CommandBuffer() = default;
		
		virtual void AddMesh(Ref<Resource> resource, Vertex vertex) = 0; 

		virtual void Begin() = 0;
		virtual void Draw() = 0;
		virtual void End() = 0;
	};
}