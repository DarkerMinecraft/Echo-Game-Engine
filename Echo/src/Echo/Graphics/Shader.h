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

		virtual const BufferLayout& GetVertexLayout() const = 0; 
		virtual const std::vector<ShaderResourceBinding> GetResourceBindings() const = 0;

		virtual const std::string& GetName() const = 0;
		virtual bool IsCompute() = 0;

		static Ref<Shader> Create(const std::filesystem::path shaderPath, bool shouldRecompile = false, bool* didCompiler = nullptr);
	};
}