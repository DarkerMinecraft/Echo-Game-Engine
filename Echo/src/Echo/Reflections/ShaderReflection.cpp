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

	void ShaderReflection::AddAttribute(const ShaderAttribute& attribute)
	{
		m_Attributes.push_back(attribute);
	}

	BufferLayout ShaderReflection::GetVertexLayout() const
	{
		BufferLayout layout;

		std::vector<BufferElement> elements;

		// Sort attributes by location to ensure correct order
		std::vector<ShaderAttribute> sortedAttributes = m_Attributes;
		std::sort(sortedAttributes.begin(), sortedAttributes.end(),
				  [](const ShaderAttribute& a, const ShaderAttribute& b)
		{
			return a.Location < b.Location;
		});

		for (const auto& attribute : sortedAttributes)
		{
			elements.emplace_back(attribute.Type, attribute.Name);
		}
	
		return BufferLayout(elements);
	}
}