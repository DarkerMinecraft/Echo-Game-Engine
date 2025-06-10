#include "pch.h"
#include "ShaderCompiler.h"

#include "Vulkan/VulkanRenderCaps.h"

using namespace slang;
namespace Echo
{


	static const char* ShaderStageToString(ShaderStage stage)
	{
		switch (stage)
		{
			case ShaderStage::Vertex:   return "Vertex";
			case ShaderStage::Fragment: return "Fragment";
			case ShaderStage::Compute:  return "Compute";
			case ShaderStage::Geometry: return "Geometry";
			case ShaderStage::All:      return "All";
			default:                    return "Unknown";
		}
	}

	static const char* ShaderDataTypeToString(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::None:   return "None";
			case ShaderDataType::Float:  return "Float";
			case ShaderDataType::Float2: return "Float2";
			case ShaderDataType::Float3: return "Float3";
			case ShaderDataType::Float4: return "Float4";
			case ShaderDataType::Int:    return "Int";
			case ShaderDataType::Int2:   return "Int2";
			case ShaderDataType::Int3:   return "Int3";
			case ShaderDataType::Int4:   return "Int4";
			case ShaderDataType::Mat3:   return "Mat3";
			case ShaderDataType::Mat4:   return "Mat4";
			case ShaderDataType::Bool:   return "Bool";
			default:                     return "Unknown";
		}
	}

	static const char* DescriptorTypeToString(DescriptorType type)
	{
		switch (type)
		{
			case DescriptorType::UniformBuffer: return "UniformBuffer";
			case DescriptorType::StorageBuffer: return "StorageBuffer";
			case DescriptorType::SampledImage:  return "SampledImage";
			case DescriptorType::StorageImage:  return "StorageImage";
			default:                            return "Unknown";
		}
	}

	ShaderLibrary::ShaderLibrary(VkDevice device)
		: m_Device(device)
	{
		createGlobalSession(m_GlobalSession.writeRef());
	}

	ShaderLibrary::~ShaderLibrary()
	{

	}

	std::vector<VkShaderModule> ShaderLibrary::AddSpirvShader(const std::filesystem::path& path, bool shouldRecompile, ShaderReflection* reflection, bool* didCompile)
	{
		EC_PROFILE_FUNCTION();
		std::vector<VkShaderModule> modules;

		UUID shaderID = UUID(); // You'll need to generate or retrieve a proper ID
		Ref<ShaderCache> cache = LoadShaderCache(path, shaderID);

		if (cache && !shouldRecompile)
		{
			EC_CORE_INFO("Using cached shader: {0}", path.string());

			// Copy reflection data
			*reflection = cache->GetReflection();

			// Create shader modules from cached SPIR-V
			const auto& shaderSprivs = cache->GetShaderSpirv();

			for (size_t i = 0; i < shaderSprivs.size(); i++)
			{
				const auto& spirv = shaderSprivs[i];

				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.codeSize = spirv.Size;
				createInfo.pCode = spirv.Bytes;

				VkShaderModule module;
				if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &module) != VK_SUCCESS)
				{
					EC_CORE_CRITICAL("Failed to create shader module from cache!");
				}
				else
				{
					modules.push_back(module);
				}
			}

			if (!modules.empty())
			{
				return modules;
			}
		}

		if (didCompile) *didCompile = false;
		EC_CORE_INFO("Compiling shader: {0}", path.string());

		SessionDesc sessionDesc{};
		TargetDesc targetDesc{};
		targetDesc.format = SLANG_SPIRV;
		targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		std::vector<slang::CompilerOptionEntry> options;
		options.push_back({
			slang::CompilerOptionName::VulkanUseEntryPointName,
			{slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
						  });
		options.push_back({
			slang::CompilerOptionName::Optimization,
			{slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr}
						  });
		sessionDesc.compilerOptionEntries = options.data();
		sessionDesc.compilerOptionEntryCount = static_cast<uint32_t>(options.size());

		Slang::ComPtr<ISession> session;
		m_GlobalSession->createSession(sessionDesc, session.writeRef());

		Slang::ComPtr<IModule> slangModule;
		{
			Slang::ComPtr<IBlob> diagnosticBlob;

			slangModule = session->loadModule(path.string().c_str(), diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::string message = (char*)diagnosticBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to load shader: {0}", message);
				if (didCompile) *didCompile = false;
			}
		}

		cache = CreateRef<ShaderCache>(shaderID);
		if (slangModule)
		{
			uint32_t entryPointCount = slangModule->getDefinedEntryPointCount();
			for (int i = 0; i < entryPointCount; i++)
			{
				IEntryPoint* entryPoint;
				{
					slangModule->getDefinedEntryPoint(i, &entryPoint);
					if (!entryPoint)
					{
						EC_CORE_CRITICAL("Failed to find entry point in {0}: ", path.string().c_str());
						if (didCompile) *didCompile = false;
					}
				}

				std::vector<IComponentType*> componentTypes =
				{
					slangModule,
				};
				componentTypes.push_back(entryPoint);

				Slang::ComPtr<IComponentType> composedProgram;
				{
					Slang::ComPtr<IBlob> diagnosticBlob;
					session->createCompositeComponentType(
						componentTypes.data(),
						componentTypes.size(),
						composedProgram.writeRef(),
						diagnosticBlob.writeRef());

					if (diagnosticBlob)
					{
						std::string message = (char*)diagnosticBlob->getBufferPointer();
						EC_CORE_CRITICAL("Failed to create composite component type: {0}", message);
						if (didCompile) *didCompile = false;
					}
				}

				Slang::ComPtr<slang::IComponentType> linkedProgram;
				{
					Slang::ComPtr<slang::IBlob> diagnosticsBlob;
					SlangResult result = composedProgram->link(
						linkedProgram.writeRef(),
						diagnosticsBlob.writeRef());

					if (diagnosticsBlob)
					{
						std::string message = (char*)diagnosticsBlob->getBufferPointer();
						EC_CORE_CRITICAL("Failed to link program: {0}", message);
						if (didCompile) *didCompile = false;
					}
				}
				Slang::ComPtr<slang::IBlob> spirvCode;
				{
					Slang::ComPtr<slang::IBlob> diagnosticsBlob;
					SlangResult result = linkedProgram->getEntryPointCode(
						0, // entryPointIndex
						0, // targetIndex
						spirvCode.writeRef(),
						diagnosticsBlob.writeRef());

					if (diagnosticsBlob)
					{
						std::string message = (char*)diagnosticsBlob->getBufferPointer();
						EC_CORE_CRITICAL("Failed to get entry point code: {0}", message);
						if (didCompile) *didCompile = false;
					}
				}

				if (!spirvCode || spirvCode->getBufferSize() == 0)
				{
					EC_CORE_CRITICAL("Empty SPIR-V code generated");
					if (didCompile) *didCompile = false;
				}

				VkShaderModuleCreateInfo createInfo{};
				createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
				createInfo.pNext = nullptr;
				createInfo.codeSize = spirvCode->getBufferSize();
				createInfo.pCode = reinterpret_cast<const uint32_t*>(spirvCode->getBufferPointer());

				// Allocate new memory and make a deep copy of the SPIRV data
				uint32_t* dataCopy = new uint32_t[spirvCode->getBufferSize() / sizeof(uint32_t)];

				// Copy the data
				std::memcpy(dataCopy, reinterpret_cast<const uint32_t*>(spirvCode->getBufferPointer()), spirvCode->getBufferSize());

				// Validate the magic number
				if (dataCopy[0] != 0x07230203)
				{
					EC_CORE_ERROR("Invalid SPIRV magic number when adding module: {0:x}", dataCopy[0]);
					delete[] dataCopy;
					if (didCompile) *didCompile = false;
				}

				VkShaderModule module;
				if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &module) != VK_SUCCESS)
				{
					EC_CORE_CRITICAL("Failed to create shader module!");
					if (didCompile) *didCompile = false;
				}

				slang::ProgramLayout* layout = linkedProgram->getLayout();
				slang::EntryPointReflection* entryPointReflection = layout->getEntryPointByIndex(0);
				ShaderStage shaderStage = SlangStageToShaderStage(entryPointReflection->getStage());

				cache->AddShaderModule(spirvCode->getBufferSize(), dataCopy);

				reflection->AddEntryPointData({ shaderStage, entryPointReflection->getName() });

				EC_CORE_INFO("Shader Reflection: {0}", path.stem().string().c_str())
					EC_CORE_INFO("  Entry Point: {0}", entryPointReflection->getName())
					EC_CORE_INFO("  Stage: {0}", ShaderStageToString(shaderStage))

					if (shaderStage == ShaderStage::Vertex)
					{
						ExtractVertexAttributes(entryPointReflection, reflection);
					}
				IMetadata* metadata;
				linkedProgram->getEntryPointMetadata(
					0,
					0,
					&metadata);
				ExtractBuffers(shaderStage, layout, metadata, reflection);

				modules.push_back(module);
			}

			cache->SetReflectionData(*reflection);
			SaveShaderCache(path, cache);
			if (didCompile) *didCompile = true;
			return modules;
		}
		else
		{
			if (didCompile) *didCompile = false;
			return modules;
		}
	}

	Ref<ShaderCache> ShaderLibrary::LoadShaderCache(const std::filesystem::path& path, const UUID& shaderID) const
	{
		EC_PROFILE_FUNCTION();
		std::filesystem::path cachePath = path.string() + ".cache";
		if (!std::filesystem::exists(cachePath))
			return nullptr;

		Ref<ShaderCache> cache = CreateRef<ShaderCache>(shaderID);
		std::ifstream stream(cachePath, std::ios::binary);

		if (!stream.is_open() || !cache->Deserialize(stream))
			return nullptr;

		return cache;
	}

	bool ShaderLibrary::SaveShaderCache(const std::filesystem::path& path, const Ref<ShaderCache>& cache) const
	{
		EC_PROFILE_FUNCTION();
		std::filesystem::path cachePath = path.string() + ".cache";
		std::filesystem::create_directories(cachePath.parent_path());

		std::ofstream stream(cachePath, std::ios::binary);
		if (!stream.is_open())
		{
			EC_CORE_ERROR("Failed to open shader cache file for writing: {0}", cachePath.string());
			return false;
		}

		if (!cache->Serialize(stream))
		{
			EC_CORE_ERROR("Failed to serialize shader cache: {0}", cachePath.string());
			return false;
		}

		EC_CORE_INFO("Shader cache saved: {0}", cachePath.string());
		return true;
	}

	void ShaderLibrary::ExtractVertexAttributes(slang::EntryPointReflection* entryPoint, ShaderReflection* reflection)
	{
		EC_PROFILE_FUNCTION();
		EC_CORE_INFO("  Vertex Attributes:");
		BufferLayout layout;

		uint32_t paramCount = entryPoint->getParameterCount();
		for (uint32_t paramIndex = 0; paramIndex < paramCount; paramIndex++)
		{
			auto param = entryPoint->getParameterByIndex(paramIndex);
			auto paramType = param->getType();
			if (param->getCategory() == slang::ParameterCategory::VaryingInput)
			{
				if (paramType->getKind() == slang::TypeReflection::Kind::Struct)
				{
					uint32_t fieldCount = paramType->getFieldCount();
					for (uint32_t fieldIndex = 0; fieldIndex < fieldCount; fieldIndex++)
					{
						auto field = paramType->getFieldByIndex(fieldIndex);

						const char* fieldName = field->getName();
						ShaderDataType fieldType = SlangTypeToShaderDataType(field->getType());

						layout.AddElement({ fieldType, fieldName });

						EC_CORE_INFO("    - {0}: {1}", fieldName, ShaderDataTypeToString(fieldType))
					}
				}
			}
		}

		reflection->SetBufferLayout(layout);
	}

	void ShaderLibrary::ExtractBuffers(ShaderStage stage, slang::ProgramLayout* layout, IMetadata* metadata, ShaderReflection* reflection)
	{
		EC_PROFILE_FUNCTION();
		EC_CORE_INFO("  Resource Bindings:");
		// 1) Get the program layout so we can fetch the *global* scope:
		auto globalParamsVarLayout = layout->getGlobalParamsVarLayout();

		// 2) Unwrap any ParameterBlock<> / ConstantBuffer<> around your struct:
		{
			auto tLayout = globalParamsVarLayout->getTypeLayout();
			if (tLayout->getKind() == slang::TypeReflection::Kind::ParameterBlock)
			{
				globalParamsVarLayout = tLayout->getElementVarLayout();
				tLayout = globalParamsVarLayout->getTypeLayout();
			}
			if (tLayout->getKind() == slang::TypeReflection::Kind::ConstantBuffer)
			{
				globalParamsVarLayout = tLayout->getContainerVarLayout();
			}
		}

		// 3) Now iterate each field in that struct:
		auto structLayout = globalParamsVarLayout->getTypeLayout();
		for (uint32_t i = 0; i < structLayout->getFieldCount(); ++i)
		{
			auto field = structLayout->getFieldByIndex(i);

			uint32_t space = field->getOffset(slang::ParameterCategory::SubElementRegisterSpace);
			uint32_t offset = field->getOffset(slang::ParameterCategory::DescriptorTableSlot);

			// 4) **Filter**: only keep it if this entry point actually uses it:
			bool isUsed = false;
			metadata->isParameterLocationUsed(
				SlangParameterCategory::SLANG_PARAMETER_CATEGORY_DESCRIPTOR_TABLE_SLOT,
				space, offset,
				isUsed);
			if (!isUsed)
				continue;

			// 5) Extract binding, set, stage, etc.
			const char* name = field->getVariable()->getName();
			uint32_t    binding = field->getBindingIndex();
			uint32_t    set = field->getBindingSpace();
			auto        type = field->getTypeLayout()->getType();

			ShaderResourceBinding rbo;

			if (type->getKind() == slang::TypeReflection::Kind::SamplerState)
			{
				rbo.Name = name;
				rbo.Binding = binding;
				rbo.Set = set;
				rbo.Stage = stage;
				rbo.Count = 1;
				rbo.Type = DescriptorType::SampledImage;
				reflection->AddResourceBinding(rbo);

				TextureBinding texBinding;
				texBinding.Name = name;
				texBinding.Type = DescriptorType::SampledImage;
				texBinding.Binding = binding;
				texBinding.Set = set;
				texBinding.Count = rbo.Count;
				texBinding.Stage = stage;
				texBinding.IsArray = type->isArray();
				reflection->AddTextureBinding(texBinding);

				EC_CORE_INFO("      Texture: {0} (set: {1}, binding: {2}, count: {3})",
							 name, set, binding, texBinding.Count);
				if (texBinding.IsArray)
				{
					EC_CORE_INFO("        Array[{0}]", texBinding.Count);
				}
			}
			else if (type->getKind() == slang::TypeReflection::Kind::ConstantBuffer)
			{
				rbo.Name = name;
				rbo.Binding = binding;
				rbo.Set = set;
				rbo.Stage = stage;
				rbo.Count = 1;
				rbo.Type = DescriptorType::UniformBuffer;
				reflection->AddResourceBinding(rbo);

				ExtractUniformBufferMembers(name, field->getTypeLayout(), reflection);
			}
			else
			{
				rbo.Name = name;
				rbo.Set = set;
				rbo.Binding = binding;
				rbo.Stage = stage;
				rbo.Count = 1;

				auto shape = type->getResourceShape();
				auto access = type->getResourceAccess();

				if (shape == SlangResourceShape::SLANG_TEXTURE_2D)
					rbo.Type = (access == SLANG_RESOURCE_ACCESS_READ)
					? DescriptorType::SampledImage
					: DescriptorType::StorageImage;
				else if (shape == SlangResourceShape::SLANG_STRUCTURED_BUFFER)
					rbo.Type = DescriptorType::StorageBuffer;

				if (type->isArray()) 
				{
					if (type->getTotalArrayElementCount() == 0) 
					{
						if (rbo.Type == DescriptorType::SampledImage) 
						{
							rbo.Count = VulkanRenderCaps::GetMaxTextureSlots();
						}
					}
					else
					{
						rbo.Count = type->getTotalArrayElementCount();
					}
				}
				
				if (shape == SlangResourceShape::SLANG_TEXTURE_2D) 
				{
					TextureBinding texBinding;
					texBinding.Name = name;
					texBinding.Type = (access == SLANG_RESOURCE_ACCESS_READ)
						? DescriptorType::SampledImage
						: DescriptorType::StorageImage;
					texBinding.Binding = binding;
					texBinding.Set = set;
					texBinding.Count = rbo.Count;
					texBinding.Stage = stage;
					texBinding.IsArray = type->isArray();
					reflection->AddTextureBinding(texBinding);

					EC_CORE_INFO("      Texture: {0} (set: {1}, binding: {2}, count: {3})",
								 name, set, binding, texBinding.Count);
					if (texBinding.IsArray)
					{
						EC_CORE_INFO("        Array[{0}]", texBinding.Count);
					}
				}

				reflection->AddResourceBinding(rbo);
			}

			EC_CORE_INFO("      Name: {0}", name);
			EC_CORE_INFO("      Type: {0}", DescriptorTypeToString(rbo.Type));
			EC_CORE_INFO("      Set: {0}, Binding: {1}", set, binding);
			EC_CORE_INFO("      Count: {0}", rbo.Count);
			EC_CORE_INFO("      Stage: {0}", ShaderStageToString(stage));
	}
}


void ShaderLibrary::ExtractUniformBufferMembers(const char* bufferName, slang::TypeLayoutReflection* bufferTypeLayout, ShaderReflection* reflection)
{
	EC_PROFILE_FUNCTION();

	// Get the element type (the actual struct inside the ConstantBuffer<>)
	auto elementTypeLayout = bufferTypeLayout->getElementTypeLayout();
	if (!elementTypeLayout)
		return;

	UniformBufferLayout uniformLayout;
	uniformLayout.BufferName = bufferName;

	EC_CORE_INFO("    Uniform Buffer '{0}' Members:", bufferName);

	// Extract all fields from the struct
	uint32_t fieldCount = elementTypeLayout->getFieldCount();
	for (uint32_t i = 0; i < fieldCount; i++)
	{
		auto fieldLayout = elementTypeLayout->getFieldByIndex(i);
		auto fieldType = fieldLayout->getType();

		UniformBufferMember member;
		member.Name = fieldLayout->getVariable()->getName();
		member.Type = SlangTypeToShaderDataType(fieldType);
		member.Offset = (uint32_t)fieldLayout->getOffset();
		member.Size = (uint32_t)fieldLayout->getBindingSpace();

		if (fieldType->isArray())
		{
			member.ArrayCount = fieldType->getTotalArrayElementCount();
		}

		uniformLayout.Members.push_back(member);

		EC_CORE_INFO("      - {0}: {1} (offset: {2}, size: {3})",
					 member.Name,
					 ShaderDataTypeToString(member.Type),
					 member.Offset,
					 member.Size);

		if (member.ArrayCount > 1)
		{
			EC_CORE_INFO("        Array[{0}]", member.ArrayCount);
		}
	}

	// Add to reflection
	reflection->AddUniformLayout(uniformLayout);
}

ShaderStage ShaderLibrary::SlangStageToShaderStage(SlangStage stage)
{
	switch (stage)
	{
		case SLANG_STAGE_VERTEX:
			return ShaderStage::Vertex;
		case SLANG_STAGE_FRAGMENT:
			return ShaderStage::Fragment;
		case SLANG_STAGE_COMPUTE:
			return ShaderStage::Compute;
		case SLANG_STAGE_GEOMETRY:
			return ShaderStage::Geometry;
		default:
			EC_CORE_WARN("Couldn't find the correct shader stage!")
				return ShaderStage::All;
	}
}

ShaderDataType ShaderLibrary::SlangTypeToShaderDataType(slang::TypeReflection* type)
{
	switch (type->getScalarType())
	{
		case slang::TypeReflection::ScalarType::Float32:
			switch (type->getRowCount())
			{
				case 1:
					switch (type->getColumnCount())
					{
						case 1: return ShaderDataType::Float;
						case 2: return ShaderDataType::Float2;
						case 3: return ShaderDataType::Float3;
						case 4: return ShaderDataType::Float4;
					}
					break;
				case 3: return ShaderDataType::Mat3;
				case 4: return ShaderDataType::Mat4;
			}
			break;
		case slang::TypeReflection::ScalarType::Int32:
			switch (type->getColumnCount())
			{
				case 1: return ShaderDataType::Int;
				case 2: return ShaderDataType::Int2;
				case 3: return ShaderDataType::Int3;
				case 4: return ShaderDataType::Int4;
			}
			break;
		case slang::TypeReflection::ScalarType::Bool:
			return ShaderDataType::Bool;
	}
	return ShaderDataType::None;
}
}