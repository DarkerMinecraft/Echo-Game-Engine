#pragma once

#include "Resource.h"
#include "Model.h"

namespace Echo 
{
	class VertexBuffer 
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() = 0;
	};

	class CommandBuffer 
	{
	public:
		virtual ~CommandBuffer() = default;
		
		virtual void AddMesh(Ref<Resource> resource, Ref<Model> model) = 0; 

		virtual void Begin() = 0;
		virtual void Submit() = 0;
		virtual void End() = 0;
	};
}