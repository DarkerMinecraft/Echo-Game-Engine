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

		std::vector<VkShaderModule> AddSpirvShader(const std::filesystem::path& path, ShaderReflection* reflection);
	private:		
		void ExtractVertexAttributes(slang::EntryPointReflection* entryPoint, ShaderReflection* reflection);
		void ExtractBuffers(ShaderStage stage, slang::ProgramLayout* layout, IMetadata* entryPointMetadata, ShaderReflection* reflection);

		ShaderStage SlangStageToShaderStage(SlangStage stage);
		ShaderDataType SlangTypeToShaderDataType(slang::TypeReflection* type);
	private:
		VkDevice m_Device;
		Slang::ComPtr<IGlobalSession> m_GlobalSession;
	};
}