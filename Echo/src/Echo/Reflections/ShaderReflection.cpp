#include "pch.h"
#include "ShaderReflection.h"

namespace Echo
{
	void ShaderReflection::AddResourceBinding(const ShaderResourceBinding& binding)
	{
		m_ResourceBindings.push_back(binding);
	}

	void ShaderReflection::AddUniformBuffer(const ShaderUniformBuffer& uniformBuffer)
	{
		m_UniformBuffers.push_back(uniformBuffer);
	}

}