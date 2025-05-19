#pragma once

#include "Serializer/Binary/IBinarySerializer.h"

#include "Core/UUID.h"

namespace Echo 
{

	class ShaderCache : public IBinarySerializer
	{
	public:
		ShaderCache(const UUID& shaderID);
		~ShaderCache();

		virtual uint32_t GetVersion() override;

		virtual bool Serialize(std::ostream& stream) override;
		virtual bool Deserialize(std::ifstream& stream) override;
		
		bool IsValid() const;
		bool IsOutdated() const;
		void AddDependency(const std::filesystem::path& path);
		void AddShaderModule(const std::vector<uint32_t>& spirvData, ShaderStage stage);
		void SetReflectionData(const ShaderReflection& reflection);

		const std::vector<std::vector<uint32_t>>& GetShaderModules() const { return m_ShaderModules; }
		const std::vector<ShaderStage>& GetShaderStages() const { return m_ShaderStages; }
		const ShaderReflection& GetReflection() const { return m_Reflection; }
	private:
		static constexpr uint32_t SHADER_CACHE_VERSION = 1;
		static constexpr uint32_t SHADER_CACHE_MAGIC = 0x43534345; // "ECSC" in ASCII

		UUID m_ShaderID;
		std::unordered_map<std::filesystem::path, long long> m_FileTimestamps;
		std::vector<std::vector<uint32_t>> m_ShaderModules; // SPIR-V binary data
		std::vector<ShaderStage> m_ShaderStages;
		ShaderReflection m_Reflection;

		bool ValidateDependencies() const;
	};

}