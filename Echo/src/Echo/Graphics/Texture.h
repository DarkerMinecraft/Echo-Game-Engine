#pragma once

#include "Echo/Core/Base.h"

#include "Pipeline.h"

#include <string>

namespace Echo 
{

	class Texture 
	{
	public:
		virtual ~Texture() = default;

		virtual void Bind(Pipeline* pipeline) = 0;

		static Ref<Texture> Create(const std::string& texturePath);
	};

}