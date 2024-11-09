#pragma once

#include "Interface/IShaderModule.h"
#include "Echo/Core/Base.h"
#include <string>

namespace Echo 
{
	class GraphicsShader 
	{
	public:
		virtual ~GraphicsShader() {}
		
		virtual IShaderModule* GetVertexShaderModule() = 0;
		virtual IShaderModule* GetFragmentShaderModule() = 0;

		virtual void Destroy() = 0;

		static Ref<GraphicsShader> Create(const std::string& vertexShader, const std::string& fragmentShader);
	};
}