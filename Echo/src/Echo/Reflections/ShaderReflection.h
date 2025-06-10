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
		uint32_t Set;
		uint32_t Count;
		ShaderStage Stage;
		DescriptorType Type;
		std::string Name;
	};

	struct UniformBufferMember
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Offset;
		uint32_t Size;
		uint32_t ArrayCount = 1; // For arrays
	};

	struct UniformBufferLayout
	{
		std::string BufferName;
		std::vector<UniformBufferMember> Members;
	};

	struct TextureBinding
	{
		std::string Name;
		DescriptorType Type;
		uint32_t Binding;
		uint32_t Set;
		uint32_t Count;
		ShaderStage Stage;
		bool IsArray;
	};

	struct EntryPointData 
	{
		ShaderStage Stage;
		std::string EntryPointName;
	};

	class ShaderReflection
	{
	public:
		ShaderReflection() = default;
		~ShaderReflection() = default;

		void SetBufferLayout(BufferLayout layout) { m_BufferLayout = layout; }
		void AddEntryPointData(const EntryPointData& data) { m_EntryPointData.push_back(data); }
		void AddResourceBinding(const ShaderResourceBinding& binding) { m_ResourceBindings.push_back(binding); }
		void AddTextureBinding(const TextureBinding& binding) { m_TextureBindings.push_back(binding); }
		void AddUniformLayout(const UniformBufferLayout& layout) { m_UniformLayouts.push_back(layout); }

		void SetParamStage(uint32_t index, const ShaderStage& stage) { m_ResourceBindings[index].Stage = stage; }

		const BufferLayout& GetVertexLayout() const { return m_BufferLayout; };
		const std::vector<EntryPointData>& GetEntryPointData() const { return m_EntryPointData; }
		const std::vector<ShaderResourceBinding>& GetResourceBindings() const { return m_ResourceBindings; }
		const std::vector<TextureBinding>& GetTextureBindings() const { return m_TextureBindings; }
		const std::vector<UniformBufferLayout>& GetUniformLayouts() const { return m_UniformLayouts; }
	private:
		BufferLayout m_BufferLayout;

		std::vector<EntryPointData> m_EntryPointData;
		std::vector<ShaderResourceBinding> m_ResourceBindings;
		std::vector<UniformBufferLayout> m_UniformLayouts;
		std::vector<TextureBinding> m_TextureBindings;
	};
}