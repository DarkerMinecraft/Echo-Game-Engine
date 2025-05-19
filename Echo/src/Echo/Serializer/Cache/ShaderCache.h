#pragma once

#include "Serializer/Binary/IBinarySerializer.h"
#include "Core/UUID.h"
#include "Graphics/RHISpecification.h"

#include "Reflections/ShaderReflection.h"

namespace Echo 
{

	struct ShaderSpirv 
	{
		size_t Size;
		const uint32_t* Bytes;
	};

	class ShaderCache : public IBinarySerializer
	{
	public:
		ShaderCache(const UUID& shaderID)
			: m_ShaderID(shaderID)
		{}
		~ShaderCache() = default;

		virtual uint32_t GetVersion() override { return SHADER_CACHE_VERSION;  }

		virtual bool Serialize(std::ostream& stream) override;
		virtual bool Deserialize(std::ifstream& stream) override;
		
		void AddShaderModule(size_t sprivSize, const uint32_t* spirvData) { m_ShaderSpirvs.push_back({ sprivSize, spirvData }); };
		void SetReflectionData(const ShaderReflection& reflection) { m_Reflection = reflection; };

		const std::vector<ShaderSpirv>& GetShaderSpirv() const { return m_ShaderSpirvs; }
		const ShaderReflection& GetReflection() const { return m_Reflection; }
	private:
		const uint32_t SHADER_CACHE_VERSION = 1;
		const uint32_t SHADER_CACHE_MAGIC = 0x43534345;

		UUID m_ShaderID;
		std::vector<ShaderSpirv> m_ShaderSpirvs;
		ShaderReflection m_Reflection;
	};

}