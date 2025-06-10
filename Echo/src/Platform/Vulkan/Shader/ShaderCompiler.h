#pragma once

#include "Reflections/ShaderReflection.h"

#include "Core/UUID.h"
#include "Serializer/Cache/ShaderCache.h"

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include <vulkan/vulkan.h>
#include <filesystem>


using namespace slang;
namespace Echo
{

	class ShaderLibrary
	{
	public:
		ShaderLibrary(VkDevice device);
		~ShaderLibrary();

		std::vector<VkShaderModule> AddSpirvShader(const std::filesystem::path& path, bool shouldRecompile, ShaderReflection* reflection, bool* didCompile);

		Ref<ShaderCache> LoadShaderCache(const std::filesystem::path& path, const UUID& shaderID) const;
		bool SaveShaderCache(const std::filesystem::path& path, const Ref<ShaderCache>& cache) const;
	private:		
		void ExtractVertexAttributes(slang::EntryPointReflection* entryPoint, ShaderReflection* reflection);
		void ExtractBuffers(ShaderStage stage, slang::ProgramLayout* layout, IMetadata* entryPointMetadata, ShaderReflection* reflection);
		void ExtractUniformBufferMembers(const char* bufferName, slang::TypeLayoutReflection* bufferTypeLayout, ShaderReflection* reflection);

		ShaderStage SlangStageToShaderStage(SlangStage stage);
		ShaderDataType SlangTypeToShaderDataType(slang::TypeReflection* type);
	private:
		VkDevice m_Device;
		Slang::ComPtr<IGlobalSession> m_GlobalSession;
	};
}