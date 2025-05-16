#pragma once

#include "Graphics/RHISpecification.h"

#include <string>
#include <vector>
#include <unordered_map>

namespace Echo
{
	struct ShaderResourceBinding
	{
		uint32_t Binding;
		uint32_t Count;
		ShaderStage Stage;
		DescriptorType Type;
		std::string Name;
	};

	struct ShaderUniformBuffer
	{
		std::string Name;
		uint32_t Binding;
		ShaderStage Stage;
	};

	class ShaderReflection
	{
	public:
		ShaderReflection() = default;
		~ShaderReflection() = default;

		void SetShaderStage(ShaderStage shaderStage) { m_ShaderStage = shaderStage; }
		void SetBufferLayout(BufferLayout layout) { m_BufferLayout = layout; }
		void SetEntryPointName(const char* entryPointName) { m_EntryPointName = entryPointName; }
		void AddResourceBinding(const ShaderResourceBinding& binding);
		void AddUniformBuffer(const ShaderUniformBuffer& uniformBuffer);

		const ShaderStage GetShaderStage() { return m_ShaderStage; }
		const BufferLayout GetVertexLayout() const { return m_BufferLayout; };
		const char* GetEntryPointName() { return m_EntryPointName; }
		const std::vector<ShaderResourceBinding>& GetResourceBindings() const { return m_ResourceBindings; }
		const std::vector<ShaderUniformBuffer>& GetUniformBuffers() const { return m_UniformBuffers; }
	private:
		ShaderStage m_ShaderStage = ShaderStage::All;
		BufferLayout m_BufferLayout;
		const char* m_EntryPointName;

		std::vector<ShaderResourceBinding> m_ResourceBindings;
		std::vector<ShaderUniformBuffer> m_UniformBuffers;
	};
}