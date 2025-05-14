#pragma once

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include <vulkan/vulkan.h>
#include "ShaderReflection.h"

using namespace slang;
namespace Echo
{
	class ShaderLibrary
	{
	public:
		ShaderLibrary(VkDevice device);
		~ShaderLibrary();

		VkShaderModule AddSpirvShader(const std::string& path);
		VkShaderModule AddSpirvShader(const char* source, const char* name);

		// New method to get shader reflection
		ShaderReflection ReflectShader(const std::string& path);
		ShaderReflection ReflectShader(const char* source, const char* name);

	private:
		ShaderReflection ExtractReflection(slang::IComponentType* program);
		ShaderDataType SlangTypeToShaderDataType(slang::TypeReflection* type);
		DescriptorType SlangBindingTypeToDescriptorType(slang::BindingType bindingType);
		ShaderStage SlangStageToShaderStage(SlangStage stage);

		VkDevice m_Device;
		Slang::ComPtr<IGlobalSession> m_GlobalSession;
	};
}