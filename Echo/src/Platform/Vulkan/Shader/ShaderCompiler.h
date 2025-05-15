#pragma once

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include <vulkan/vulkan.h>
#include <filesystem>

#include "Reflections/ShaderReflection.h"

using namespace slang;
namespace Echo
{
	class ShaderLibrary
	{
	public:
		ShaderLibrary(VkDevice device);
		~ShaderLibrary();

		VkShaderModule AddSpirvShader(const std::filesystem::path& path);
		VkShaderModule AddSpirvShader(const char* source, const char* name);

		ShaderReflection ReflectShader(const std::filesystem::path& path);
		ShaderReflection ReflectShader(const char* source, const char* name);
	private:
		ShaderReflection ExtractReflection(slang::IComponentType* program);
		
		void ExtractVertexAttributes(slang::EntryPointReflection* entryPoint, ShaderReflection& reflection);

		ShaderStage SlangStageToShaderStage(SlangStage stage);
		ShaderDataType SlangTypeToShaderDataType(slang::TypeReflection* type);
		uint32_t ExtractLocationFromSemantic(const char* semantic);

		slang::IComponentType* CompileShader(const std::filesystem::path& path);
		Slang::ComPtr<IComponentType> CompileShader(const char* source, const char* name);
	private:
		VkDevice m_Device;
		Slang::ComPtr<IGlobalSession> m_GlobalSession;
	};
}