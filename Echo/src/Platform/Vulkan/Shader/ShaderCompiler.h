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

		void AddSpirvShader(const std::filesystem::path& path, VkShaderModule* module, ShaderReflection* reflection);
		void AddSpirvShader(const char* source, const char* name, VkShaderModule* module, ShaderReflection* reflection);
	private:
		void ExtractReflection(slang::ProgramLayout* layout, ShaderReflection* reflection);
		
		void ExtractVertexAttributes(slang::EntryPointReflection* entryPoint, ShaderReflection* reflection);
		void ExtractUniformBuffers(ShaderStage stage, slang::ProgramLayout* layout, ShaderReflection* reflection);
		void ExtractResourceBuffers(ShaderStage stage, slang::ProgramLayout* layout, ShaderReflection* reflection);

		ShaderStage SlangStageToShaderStage(SlangStage stage);
		ShaderDataType SlangTypeToShaderDataType(slang::TypeReflection* type);
	private:
		VkDevice m_Device;
		Slang::ComPtr<IGlobalSession> m_GlobalSession;
	};
}