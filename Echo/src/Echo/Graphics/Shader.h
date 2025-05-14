#pragma once

#include "RHISpecification.h"
#include "Reflections/ShaderReflection.h"

#include <unordered_map>

namespace Echo
{
	// Add shader reflection data structure
	struct ShaderResourceLayout
	{
		struct UniformBuffer
		{
			uint32_t Binding;
			uint32_t Size;
			std::string Name;
		};

		struct ShaderResource
		{
			uint32_t Binding;
			std::string Name;
			std::string Type;
		};

		std::vector<UniformBuffer> UniformBuffers;
		std::vector<ShaderResource> Resources;
	};

	class Shader 
	{
	public:
		virtual ~Shader() = default;

		virtual void Reload() = 0;
		virtual void Unload() = 0;
		virtual void Destroy() = 0;

		virtual const std::string& GetName() const = 0;
		virtual ShaderResourceLayout& GetResourceLayout() = 0;
		virtual bool IsCompute() = 0;

		virtual const ShaderReflection& GetReflection() const = 0;

		static Ref<Shader> Create(const ShaderSpecification& spec);
	};
}