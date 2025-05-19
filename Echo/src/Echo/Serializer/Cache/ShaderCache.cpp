#include "pch.h"
#include "ShaderCache.h"

namespace Echo 
{

	bool ShaderCache::Serialize(std::ofstream& stream)
	{
		// Write header
		stream.write(reinterpret_cast<const char*>(&SHADER_CACHE_MAGIC), sizeof(uint32_t));
		uint32_t version = GetVersion();
		stream.write(reinterpret_cast<const char*>(&version), sizeof(uint32_t));
		uint64_t uuid = m_ShaderID;
		stream.write(reinterpret_cast<const char*>(&uuid), sizeof(uint64_t));

		// Write dependencies
		uint32_t depCount = static_cast<uint32_t>(m_FileTimestamps.size());
		stream.write(reinterpret_cast<const char*>(&depCount), sizeof(uint32_t));
		for (const auto& [path, timestamp] : m_FileTimestamps)
		{
			std::string pathStr = path.string();
			uint32_t pathLen = static_cast<uint32_t>(pathStr.length());
			stream.write(reinterpret_cast<const char*>(&pathLen), sizeof(uint32_t));
			stream.write(pathStr.c_str(), pathLen);
			stream.write(reinterpret_cast<const char*>(&timestamp), sizeof(long long));
		}

		// Write shader modules
		uint32_t moduleCount = static_cast<uint32_t>(m_ShaderModules.size());
		stream.write(reinterpret_cast<const char*>(&moduleCount), sizeof(uint32_t));
		for (size_t i = 0; i < moduleCount; i++)
		{
			uint32_t stage = static_cast<uint32_t>(m_ShaderStages[i]);
			stream.write(reinterpret_cast<const char*>(&stage), sizeof(uint32_t));

			const auto& spirv = m_ShaderModules[i];
			uint32_t spirvSize = static_cast<uint32_t>(spirv.size() * sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(&spirvSize), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(spirv.data()), spirvSize);
		}

		// Write reflection data (buffer layout)
		const auto& layout = m_Reflection.GetVertexLayout();
		uint32_t elementCount = static_cast<uint32_t>(layout.Size());
		stream.write(reinterpret_cast<const char*>(&elementCount), sizeof(uint32_t));

		for (const auto& element : layout.GetElements())
		{
			uint32_t nameLen = static_cast<uint32_t>(element.Name.length());
			stream.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
			stream.write(element.Name.c_str(), nameLen);

			uint32_t type = static_cast<uint32_t>(element.Type);
			stream.write(reinterpret_cast<const char*>(&type), sizeof(uint32_t));

			stream.write(reinterpret_cast<const char*>(&element.Size), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(&element.Offset), sizeof(uint32_t));

			uint8_t normalized = element.Normalized ? 1 : 0;
			stream.write(reinterpret_cast<const char*>(&normalized), sizeof(uint8_t));
		}

		// Write resource bindings
		const auto& bindings = m_Reflection.GetResourceBindings();
		uint32_t bindingCount = static_cast<uint32_t>(bindings.size());
		stream.write(reinterpret_cast<const char*>(&bindingCount), sizeof(uint32_t));

		for (const auto& binding : bindings)
		{
			uint32_t nameLen = static_cast<uint32_t>(binding.Name.length());
			stream.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
			stream.write(binding.Name.c_str(), nameLen);

			stream.write(reinterpret_cast<const char*>(&binding.Binding), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(&binding.Set), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(&binding.Count), sizeof(uint32_t));

			uint32_t stage = static_cast<uint32_t>(binding.Stage);
			stream.write(reinterpret_cast<const char*>(&stage), sizeof(uint32_t));

			uint32_t type = static_cast<uint32_t>(binding.Type);
			stream.write(reinterpret_cast<const char*>(&type), sizeof(uint32_t));
		}

		return stream.good();
	}

	bool ShaderCache::Deserialize(std::ifstream& stream)
	{
		// Read and validate header
		uint32_t magic;
		stream.read(reinterpret_cast<char*>(&magic), sizeof(uint32_t));
		if (magic != SHADER_CACHE_MAGIC)
		{
			EC_CORE_ERROR("Invalid shader cache file - wrong magic number");
			return false;
		}

		uint32_t version;
		stream.read(reinterpret_cast<char*>(&version), sizeof(uint32_t));
		if (version != GetVersion())
		{
			EC_CORE_ERROR("Shader cache version mismatch - expected {0}, got {1}", GetVersion(), version);
			return false;
		}

		uint64_t uuid;
		stream.read(reinterpret_cast<char*>(&uuid), sizeof(uint64_t));
		m_ShaderID = uuid;

		// Read dependencies
		uint32_t depCount;
		stream.read(reinterpret_cast<char*>(&depCount), sizeof(uint32_t));

		m_FileTimestamps.clear();
		for (uint32_t i = 0; i < depCount; i++)
		{
			uint32_t pathLen;
			stream.read(reinterpret_cast<char*>(&pathLen), sizeof(uint32_t));

			std::string pathStr(pathLen, ' ');
			stream.read(&pathStr[0], pathLen);

			long long timestamp;
			stream.read(reinterpret_cast<char*>(&timestamp), sizeof(long long));

			m_FileTimestamps[std::filesystem::path(pathStr)] = timestamp;
		}

		// Read shader modules
		uint32_t moduleCount;
		stream.read(reinterpret_cast<char*>(&moduleCount), sizeof(uint32_t));

		m_ShaderModules.clear();
		m_ShaderStages.clear();

		for (uint32_t i = 0; i < moduleCount; i++)
		{
			uint32_t stage;
			stream.read(reinterpret_cast<char*>(&stage), sizeof(uint32_t));
			m_ShaderStages.push_back(static_cast<ShaderStage>(stage));

			uint32_t spirvSize;
			stream.read(reinterpret_cast<char*>(&spirvSize), sizeof(uint32_t));

			std::vector<uint32_t> spirv(spirvSize / sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(spirv.data()), spirvSize);

			m_ShaderModules.push_back(std::move(spirv));
		}

		// Read reflection data (buffer layout)
		uint32_t elementCount;
		stream.read(reinterpret_cast<char*>(&elementCount), sizeof(uint32_t));

		std::vector<BufferElement> elements;
		for (uint32_t i = 0; i < elementCount; i++)
		{
			uint32_t nameLen;
			stream.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));

			std::string name(nameLen, ' ');
			stream.read(&name[0], nameLen);

			uint32_t type;
			stream.read(reinterpret_cast<char*>(&type), sizeof(uint32_t));

			uint32_t size;
			stream.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));

			uint32_t offset;
			stream.read(reinterpret_cast<char*>(&offset), sizeof(uint32_t));

			uint8_t normalizedByte;
			stream.read(reinterpret_cast<char*>(&normalizedByte), sizeof(uint8_t));
			bool normalized = normalizedByte != 0;

			BufferElement element(static_cast<ShaderDataType>(type), name, normalized);
			element.Size = size;
			element.Offset = offset;
			elements.push_back(element);
		}

		BufferLayout layout(elements);

		// Read resource bindings
		uint32_t bindingCount;
		stream.read(reinterpret_cast<char*>(&bindingCount), sizeof(uint32_t));

		std::vector<ShaderResourceBinding> bindings;
		for (uint32_t i = 0; i < bindingCount; i++)
		{
			uint32_t nameLen;
			stream.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));

			std::string name(nameLen, ' ');
			stream.read(&name[0], nameLen);

			uint32_t binding, set, count;
			stream.read(reinterpret_cast<char*>(&binding), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&set), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&count), sizeof(uint32_t));

			uint32_t stage;
			stream.read(reinterpret_cast<char*>(&stage), sizeof(uint32_t));

			uint32_t type;
			stream.read(reinterpret_cast<char*>(&type), sizeof(uint32_t));

			ShaderResourceBinding resourceBinding{
				binding,
				set,
				count,
				static_cast<ShaderStage>(stage),
				static_cast<DescriptorType>(type),
				name
			};

			bindings.push_back(resourceBinding);
		}

		// Reconstruct ShaderReflection
		ShaderReflection reflection;
		reflection.SetBufferLayout(layout);

		for (const auto& binding : bindings)
		{
			reflection.AddResourceBinding(binding);
		}

		m_Reflection = reflection;

		return ValidateDependencies() && stream.good();
	}

}