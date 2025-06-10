#include "pch.h"
#include "ShaderCache.h"

namespace Echo 
{

	bool ShaderCache::Serialize(std::ostream& stream)
	{
		EC_PROFILE_FUNCTION();
		stream.write(reinterpret_cast<const char*>(&SHADER_CACHE_MAGIC), sizeof(uint32_t));
		uint32_t version = GetVersion();
		stream.write(reinterpret_cast<const char*>(&version), sizeof(uint32_t));
		uint64_t uuid = m_ShaderID;
		stream.write(reinterpret_cast<const char*>(&uuid), sizeof(uint64_t));

		// Write shader modules
		uint32_t moduleCount = static_cast<uint32_t>(m_ShaderSpirvs.size());
		stream.write(reinterpret_cast<const char*>(&moduleCount), sizeof(uint32_t));

		for (const auto& spirv : m_ShaderSpirvs)
		{
			// First verify the magic number in our data (debugging check)
			if (spirv.Bytes && spirv.Size >= 4)
			{
				uint32_t magicNumber = spirv.Bytes[0];
				if (magicNumber != 0x07230203)
				{
					EC_CORE_ERROR("Invalid SPIRV magic number when serializing: {0:x}", magicNumber);
					return false;
				}
			}

			// Write the size in bytes
			stream.write(reinterpret_cast<const char*>(&spirv.Size), sizeof(size_t));

			// Write the actual SPIRV data
			stream.write(reinterpret_cast<const char*>(spirv.Bytes), spirv.Size);
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

		const auto& entryPoints = m_Reflection.GetEntryPointData();
		uint32_t entryPointCount = static_cast<uint32_t>(entryPoints.size());
		stream.write(reinterpret_cast<const char*>(&entryPointCount), sizeof(uint32_t));

		for (const auto& entryPoint : entryPoints)
		{
			// Write shader stage
			uint32_t stage = static_cast<uint32_t>(entryPoint.Stage);
			stream.write(reinterpret_cast<const char*>(&stage), sizeof(uint32_t));

			// Write entry point name
			uint32_t nameLen = static_cast<uint32_t>(entryPoint.EntryPointName.length());
			stream.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
			stream.write(entryPoint.EntryPointName.c_str(), nameLen);
		}

		const auto& uniformLayouts = m_Reflection.GetUniformLayouts();
		uint32_t uniformLayoutCount = static_cast<uint32_t>(uniformLayouts.size());
		stream.write(reinterpret_cast<const char*>(&uniformLayoutCount), sizeof(uint32_t));

		for (const auto& layout : uniformLayouts)
		{
			// Write buffer name
			uint32_t nameLen = static_cast<uint32_t>(layout.BufferName.length());
			stream.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
			stream.write(layout.BufferName.c_str(), nameLen);

			// Write member count
			uint32_t memberCount = static_cast<uint32_t>(layout.Members.size());
			stream.write(reinterpret_cast<const char*>(&memberCount), sizeof(uint32_t));

			// Write each member
			for (const auto& member : layout.Members)
			{
				// Member name
				uint32_t memberNameLen = static_cast<uint32_t>(member.Name.length());
				stream.write(reinterpret_cast<const char*>(&memberNameLen), sizeof(uint32_t));
				stream.write(member.Name.c_str(), memberNameLen);

				// Member data
				uint32_t type = static_cast<uint32_t>(member.Type);
				stream.write(reinterpret_cast<const char*>(&type), sizeof(uint32_t));
				stream.write(reinterpret_cast<const char*>(&member.Offset), sizeof(uint32_t));
				stream.write(reinterpret_cast<const char*>(&member.Size), sizeof(uint32_t));
				stream.write(reinterpret_cast<const char*>(&member.ArrayCount), sizeof(uint32_t));
			}
		}

		// Write texture layout
		const auto& textureBindings = m_Reflection.GetTextureBindings();
		uint32_t textureCount = static_cast<uint32_t>(textureBindings.size());
		stream.write(reinterpret_cast<const char*>(&textureCount), sizeof(uint32_t));

		for (const auto& texture : textureBindings)
		{
			// Texture name
			uint32_t nameLen = static_cast<uint32_t>(texture.Name.length());
			stream.write(reinterpret_cast<const char*>(&nameLen), sizeof(uint32_t));
			stream.write(texture.Name.c_str(), nameLen);

			// Texture data
			uint32_t type = static_cast<uint32_t>(texture.Type);
			stream.write(reinterpret_cast<const char*>(&type), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(&texture.Binding), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(&texture.Set), sizeof(uint32_t));
			stream.write(reinterpret_cast<const char*>(&texture.Count), sizeof(uint32_t));

			uint32_t stage = static_cast<uint32_t>(texture.Stage);
			stream.write(reinterpret_cast<const char*>(&stage), sizeof(uint32_t));

			uint8_t isArray = texture.IsArray ? 1 : 0;
			stream.write(reinterpret_cast<const char*>(&isArray), sizeof(uint8_t));
		}

		return stream.good();
	}

	bool ShaderCache::Deserialize(std::ifstream& stream)
	{
		EC_PROFILE_FUNCTION();
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

		// Read shader modules
		uint32_t moduleCount;
		stream.read(reinterpret_cast<char*>(&moduleCount), sizeof(uint32_t));
		m_ShaderSpirvs.clear();

		for (uint32_t i = 0; i < moduleCount; i++)
		{
			// Read the size of the SPIRV data
			size_t spirvSize;
			stream.read(reinterpret_cast<char*>(&spirvSize), sizeof(size_t));

			// Calculate how many uint32_t elements we need
			uint32_t wordCount = spirvSize / sizeof(uint32_t);

			// Allocate memory for the SPIRV data as uint32_t array
			uint32_t* spirvData = new uint32_t[wordCount];

			// Read the data into the allocated memory
			stream.read(reinterpret_cast<char*>(spirvData), spirvSize);

			// Validate the SPIRV magic number
			if (spirvData[0] != 0x07230203)
			{
				EC_CORE_ERROR("Invalid SPIRV magic number in shader module {0}: {1:x}",
							  i, spirvData[0]);
				delete[] spirvData;
				return false;
			}

			// Add to the collection
			m_ShaderSpirvs.push_back({ spirvSize, spirvData });
		}

		// Read buffer layout elements
		uint32_t elementCount;
		stream.read(reinterpret_cast<char*>(&elementCount), sizeof(uint32_t));

		std::vector<BufferElement> elements;
		for (uint32_t i = 0; i < elementCount; i++)
		{
			// Read name length
			uint32_t nameLen;
			stream.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));

			// Allocate space for the name and read it
			std::vector<char> nameBuffer(nameLen + 1, 0);
			stream.read(nameBuffer.data(), nameLen);
			std::string name(nameBuffer.data(), nameLen);

			// Read remaining element data
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

		// Read resource bindings with the same string reading pattern
		uint32_t bindingCount;
		stream.read(reinterpret_cast<char*>(&bindingCount), sizeof(uint32_t));

		std::vector<ShaderResourceBinding> bindings;
		for (uint32_t i = 0; i < bindingCount; i++)
		{
			uint32_t nameLen;
			stream.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));

			// Properly read string data
			std::vector<char> nameBuffer(nameLen + 1, 0);
			stream.read(nameBuffer.data(), nameLen);
			std::string name(nameBuffer.data(), nameLen);

			// Read remaining binding data
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

		uint32_t entryPointCount;
		stream.read(reinterpret_cast<char*>(&entryPointCount), sizeof(uint32_t));

		for (uint32_t i = 0; i < entryPointCount; i++)
		{
			// Read shader stage
			uint32_t stage;
			stream.read(reinterpret_cast<char*>(&stage), sizeof(uint32_t));

			// Read entry point name
			uint32_t nameLen;
			stream.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));

			std::vector<char> nameBuffer(nameLen + 1, 0);
			stream.read(nameBuffer.data(), nameLen);
			std::string name(nameBuffer.data(), nameLen);

			// Add entry point to reflection
			m_Reflection.AddEntryPointData({ static_cast<ShaderStage>(stage), name });
		}

		// Reconstruct ShaderReflection
		m_Reflection.SetBufferLayout(layout);

		for (const auto& binding : bindings)
		{
			m_Reflection.AddResourceBinding(binding);
		}

		uint32_t uniformLayoutCount;
		stream.read(reinterpret_cast<char*>(&uniformLayoutCount), sizeof(uint32_t));

		for (uint32_t i = 0; i < uniformLayoutCount; i++)
		{
			UniformBufferLayout layout;

			// Read buffer name
			uint32_t nameLen;
			stream.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));
			std::vector<char> nameBuffer(nameLen + 1, 0);
			stream.read(nameBuffer.data(), nameLen);
			layout.BufferName = std::string(nameBuffer.data(), nameLen);

			// Read member count
			uint32_t memberCount;
			stream.read(reinterpret_cast<char*>(&memberCount), sizeof(uint32_t));

			// Read each member
			for (uint32_t j = 0; j < memberCount; j++)
			{
				UniformBufferMember member;

				// Read member name
				uint32_t memberNameLen;
				stream.read(reinterpret_cast<char*>(&memberNameLen), sizeof(uint32_t));
				std::vector<char> memberNameBuffer(memberNameLen + 1, 0);
				stream.read(memberNameBuffer.data(), memberNameLen);
				member.Name = std::string(memberNameBuffer.data(), memberNameLen);

				// Read member data
				uint32_t type;
				stream.read(reinterpret_cast<char*>(&type), sizeof(uint32_t));
				member.Type = static_cast<ShaderDataType>(type);
				stream.read(reinterpret_cast<char*>(&member.Offset), sizeof(uint32_t));
				stream.read(reinterpret_cast<char*>(&member.Size), sizeof(uint32_t));
				stream.read(reinterpret_cast<char*>(&member.ArrayCount), sizeof(uint32_t));

				layout.Members.push_back(member);
			}

			m_Reflection.AddUniformLayout(layout);
		}

		// Read texture layout
		uint32_t textureCount;
		stream.read(reinterpret_cast<char*>(&textureCount), sizeof(uint32_t));

		for (uint32_t i = 0; i < textureCount; i++)
		{
			TextureBinding texture;

			// Read texture name
			uint32_t nameLen;
			stream.read(reinterpret_cast<char*>(&nameLen), sizeof(uint32_t));
			std::vector<char> nameBuffer(nameLen + 1, 0);
			stream.read(nameBuffer.data(), nameLen);
			texture.Name = std::string(nameBuffer.data(), nameLen);

			// Read texture data
			uint32_t type;
			stream.read(reinterpret_cast<char*>(&type), sizeof(uint32_t));
			texture.Type = static_cast<DescriptorType>(type);
			stream.read(reinterpret_cast<char*>(&texture.Binding), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&texture.Set), sizeof(uint32_t));
			stream.read(reinterpret_cast<char*>(&texture.Count), sizeof(uint32_t));

			uint32_t stage;
			stream.read(reinterpret_cast<char*>(&stage), sizeof(uint32_t));
			texture.Stage = static_cast<ShaderStage>(stage);

			uint8_t isArrayByte;
			stream.read(reinterpret_cast<char*>(&isArrayByte), sizeof(uint8_t));
			texture.IsArray = isArrayByte != 0;

			m_Reflection.AddTextureBinding(texture);
		}

		return stream.good();
	}

}