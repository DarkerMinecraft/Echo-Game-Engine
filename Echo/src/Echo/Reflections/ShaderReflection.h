#pragma once

#include "Graphics/RHISpecification.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Echo
{
	struct ShaderResourceBinding
	{
		uint32_t Set;
		uint32_t Binding;
		uint32_t Count;
		uint32_t ArraySize;
		ShaderStage Stage;
		DescriptorType Type;
		std::string Name;
	};

	struct ShaderUniformBuffer
	{
		uint32_t Set;
		uint32_t Binding;
		uint32_t Size;
		ShaderStage Stage;
		std::string Name;
		std::vector<BufferElement> Elements;
	};

	struct ShaderAttribute
	{
		uint32_t Location;
		ShaderDataType Type;
		std::string Name;
	};

	class ShaderReflection
	{
	public:
		ShaderReflection() = default;
		~ShaderReflection() = default;

		void SetShaderStage(ShaderStage shaderStage) { m_ShaderStage = shaderStage; }
		void AddResourceBinding(const ShaderResourceBinding& binding);
		void AddUniformBuffer(const ShaderUniformBuffer& uniformBuffer);
		void AddAttribute(const ShaderAttribute& attribute);

		const ShaderStage& GetShaderStage() { return m_ShaderStage; }
		const std::vector<ShaderResourceBinding>& GetResourceBindings() const { return m_ResourceBindings; }
		const std::vector<ShaderUniformBuffer>& GetUniformBuffers() const { return m_UniformBuffers; }
		const std::vector<ShaderAttribute>& GetAttributes() const { return m_Attributes; }

		BufferLayout GetVertexLayout() const;
	private:
		ShaderStage m_ShaderStage = ShaderStage::All;
		std::vector<ShaderResourceBinding> m_ResourceBindings;
		std::vector<ShaderUniformBuffer> m_UniformBuffers;
		std::vector<ShaderAttribute> m_Attributes;
	};
}