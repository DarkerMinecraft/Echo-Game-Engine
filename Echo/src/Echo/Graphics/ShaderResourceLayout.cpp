#include "pch.h"
#include "ShaderResourceLayout.h"

namespace Echo
{
	void ShaderResourceLayout::AddResourceBinding(const ShaderResourceBinding& binding)
	{
		m_ResourceBindings.push_back(binding);
	}

	void ShaderResourceLayout::AddUniformBuffer(const ShaderUniformBuffer& uniformBuffer)
	{
		m_UniformBuffers.push_back(uniformBuffer);
	}

	void ShaderResourceLayout::AddAttribute(const ShaderAttribute& attribute)
	{
		m_Attributes.push_back(attribute);
	}

	void ShaderResourceLayout::Clear()
	{
		m_ResourceBindings.clear();
		m_UniformBuffers.clear();
		m_Attributes.clear();
	}

	BufferLayout ShaderResourceLayout::GetVertexLayout() const
	{
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

		return elements;
	}
}