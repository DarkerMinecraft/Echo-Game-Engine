#pragma once

#include "Echo/Core/Base.h"

#include "Resource.h"
#include <vector>

namespace Echo 
{
	class Model 
	{
	public:
		virtual void Bind() = 0;

		virtual uint32_t GetVertexCount() = 0;

		static Ref<Model> CreateModel(const std::vector<Vertex> vertices);
	};
}