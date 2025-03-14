#pragma once

#include <slang.h>
#include <slang-com-ptr.h>
#include <slang-com-helper.h>

#include <vulkan/vulkan.h>

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
	private:
		VkDevice m_Device;
		Slang::ComPtr<IGlobalSession> m_GlobalSession;
	};
}