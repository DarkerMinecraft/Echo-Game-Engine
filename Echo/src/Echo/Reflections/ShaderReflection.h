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

	struct EntryPointData 
	{
		ShaderStage Stage;
		const char* EntryPointName;
	};

	class ShaderReflection
	{
	public:
		ShaderReflection() = default;
		~ShaderReflection() = default;

		void SetBufferLayout(BufferLayout layout) { m_BufferLayout = layout; }
		void AddEntryPointData(const EntryPointData& data) { m_EntryPointData.push_back(data); }
		void AddResourceBinding(const ShaderResourceBinding& binding) { m_ResourceBindings.push_back(binding); }

		void SetParamStage(uint32_t index, const ShaderStage& stage) { m_ResourceBindings[index].Stage = stage; }

		const BufferLayout& GetVertexLayout() const { return m_BufferLayout; };
		const std::vector<EntryPointData>& GetEntryPointData() const { return m_EntryPointData; }
		const std::vector<ShaderResourceBinding>& GetResourceBindings() const { return m_ResourceBindings; }
	private:
		BufferLayout m_BufferLayout;

		std::vector<EntryPointData> m_EntryPointData;
		std::vector<ShaderResourceBinding> m_ResourceBindings;
	};
}