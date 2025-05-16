#pragma once

#include "Reflections/ShaderReflection.h"

#include <unordered_map>
#include <filesystem>

namespace Echo
{

	class Shader 
	{
	public:
		virtual ~Shader() = default;

		virtual void Unload() = 0;
		virtual void Destroy() = 0;

		virtual const std::vector<ShaderReflection>& GetReflections() const = 0;
		virtual const ShaderReflection& GetVertexReflection() const = 0;
		virtual const ShaderReflection& GetFragmentReflection() const = 0;
		virtual const ShaderReflection& GetComputeRelection() const = 0;

		virtual const std::string& GetName() const = 0;
		virtual bool IsCompute() = 0;

		static Ref<Shader> Create(const std::filesystem::path shaderPath, bool useCurrentDirectory = false);
		static Ref<Shader> Create(const std::string& shaderSource, const std::string& shaderName);
	};
}