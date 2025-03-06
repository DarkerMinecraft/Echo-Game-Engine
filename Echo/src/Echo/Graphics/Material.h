#pragma once 

#include "Echo/Core/Base.h"

namespace Echo 
{

	class Material 
	{
	public:
		virtual ~Material() = default;

		virtual void Destroy() = 0; 

		static Ref<Material> Create(const char* vertexShaderPath, const char* fragmentShaderPath, const char* geometryShaderPath = nullptr);
		static Ref<Material> Create(const char* vertexShaderSource, const char* fragmentShaderSource, const char* shaderName, const char* geometryShaderSource = nullptr);
	};

}