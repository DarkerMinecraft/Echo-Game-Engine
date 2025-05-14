#include "pch.h"
#include "ShaderCompiler.h"

using namespace slang;
namespace Echo
{

	ShaderLibrary::ShaderLibrary(VkDevice device)
		: m_Device(device)
	{
		createGlobalSession(m_GlobalSession.writeRef());
	}

	ShaderLibrary::~ShaderLibrary()
	{

	}

	VkShaderModule ShaderLibrary::AddSpirvShader(const std::string& path)
	{
		SessionDesc sessionDesc{};
		TargetDesc targetDesc{};
		targetDesc.format = SLANG_SPIRV;
		targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		Slang::ComPtr<ISession> session;
		m_GlobalSession->createSession(sessionDesc, session.writeRef());

		Slang::ComPtr<IModule> slangModule;
		{
			Slang::ComPtr<IBlob> diagnosticBlob;

			slangModule = session->loadModule(path.c_str(), diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::string message = (char*)diagnosticBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to load shader: {0}", message);
			}
		}

		Slang::ComPtr<IEntryPoint> entryPoint;
		{
			slangModule->findEntryPointByName("main", entryPoint.writeRef());
			if (!entryPoint)
			{
				EC_CORE_CRITICAL("Failed to find entry point in {0}: ", path);
			}
		}

		std::array<IComponentType*, 2> componentTypes =
		{
			slangModule,
			entryPoint
		};

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
			}
		}

		if (!spirvCode || spirvCode->getBufferSize() == 0)
		{
			EC_CORE_CRITICAL("Empty SPIR-V code generated");
		}

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.codeSize = spirvCode->getBufferSize();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(spirvCode->getBufferPointer());

		VkShaderModule module;
		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &module) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create shader module!");
		}

		return module;
	}

	VkShaderModule ShaderLibrary::AddSpirvShader(const char* source, const char* name)
	{
		SessionDesc sessionDesc{};
		TargetDesc targetDesc{};
		targetDesc.format = SLANG_SPIRV;
		targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		Slang::ComPtr<ISession> session;
		m_GlobalSession->createSession(sessionDesc, session.writeRef());

		Slang::ComPtr<IModule> slangModule;
		{
			Slang::ComPtr<IBlob> diagnosticBlob;
			std::string modulePath = std::string(name) + ".slang";
			slangModule = session->loadModuleFromSourceString(name, modulePath.c_str(), source, diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::string message = (char*)diagnosticBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to load shader: {0}", message);
			}
		}

		Slang::ComPtr<IEntryPoint> entryPoint;
		{
			slangModule->findEntryPointByName("main", entryPoint.writeRef());
			if (!entryPoint)
			{
				EC_CORE_CRITICAL("Failed to find entry point in {0}: ", name);
			}
		}

		std::array<IComponentType*, 2> componentTypes =
		{
			slangModule,
			entryPoint
		};

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
			}
		}

		if (!spirvCode || spirvCode->getBufferSize() == 0)
		{
			EC_CORE_CRITICAL("Empty SPIR-V code generated");
		}

		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.pNext = nullptr;
		createInfo.codeSize = spirvCode->getBufferSize();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(spirvCode->getBufferPointer());

		VkShaderModule module;
		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, &module) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create shader module!");
		}

		return module;
	}

	ShaderReflection ShaderLibrary::ReflectShader(const std::string& path)
	{
		SessionDesc sessionDesc{};
		TargetDesc targetDesc{};
		targetDesc.format = SLANG_SPIRV;
		targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		Slang::ComPtr<ISession> session;
		m_GlobalSession->createSession(sessionDesc, session.writeRef());

		Slang::ComPtr<IModule> slangModule;
		{
			Slang::ComPtr<IBlob> diagnosticBlob;
			slangModule = session->loadModule(path.c_str(), diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::string message = (char*)diagnosticBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to load shader: {0}", message);
				return {};
			}
		}

		Slang::ComPtr<IComponentType> linkedProgram;
		{
			Slang::ComPtr<IBlob> diagnosticBlob;
			session->createCompositeComponentType(&slangModule.get(), 1, linkedProgram.writeRef(), diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::string message = (char*)diagnosticBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to create composite: {0}", message);
				return {};
			}
		}

		{
			Slang::ComPtr<IBlob> diagnosticsBlob;
			linkedProgram->link(linkedProgram.writeRef(), diagnosticsBlob.writeRef());
			if (diagnosticsBlob)
			{
				std::string message = (char*)diagnosticsBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to link: {0}", message);
				return {};
			}
		}

		return ExtractReflection(linkedProgram);
	}

	ShaderReflection ShaderLibrary::ReflectShader(const char* source, const char* name)
	{
		SessionDesc sessionDesc{};
		TargetDesc targetDesc{};
		targetDesc.format = SLANG_SPIRV;
		targetDesc.profile = m_GlobalSession->findProfile("spirv_1_5");

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		Slang::ComPtr<ISession> session;
		m_GlobalSession->createSession(sessionDesc, session.writeRef());

		Slang::ComPtr<IModule> slangModule;
		{
			Slang::ComPtr<IBlob> diagnosticBlob;
			std::string modulePath = std::string(name) + ".slang";
			slangModule = session->loadModuleFromSourceString(name, modulePath.c_str(), source, diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::string message = (char*)diagnosticBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to load shader: {0}", message);
				return {};
			}
		}

		Slang::ComPtr<IComponentType> linkedProgram;
		{
			Slang::ComPtr<IBlob> diagnosticBlob;
			session->createCompositeComponentType(&slangModule.get(), 1, linkedProgram.writeRef(), diagnosticBlob.writeRef());
			if (diagnosticBlob)
			{
				std::string message = (char*)diagnosticBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to create composite: {0}", message);
				return {};
			}
		}

		{
			Slang::ComPtr<IBlob> diagnosticsBlob;
			linkedProgram->link(linkedProgram.writeRef(), diagnosticsBlob.writeRef());
			if (diagnosticsBlob)
			{
				std::string message = (char*)diagnosticsBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to link: {0}", message);
				return {};
			}
		}

		return ExtractReflection(linkedProgram);
	}

	ShaderReflection ShaderLibrary::ExtractReflection(slang::IComponentType* program)
	{
		ShaderReflection reflection;

		// Get the program reflection
		auto programReflection = program->getLayout();

		// Extract entry points
		uint32_t entryPointCount = program->getEntryPointCount();
		for (uint32_t i = 0; i < entryPointCount; i++)
		{
			auto entryPoint = program->getEntryPointByIndex(i);
			auto stage = SlangStageToShaderStage(entryPoint->getStage());

			// Extract entry point parameters (vertex attributes)
			auto entryPointReflection = entryPoint->getLayout();
			uint32_t paramCount = entryPointReflection->getParameterCount();

			for (uint32_t p = 0; p < paramCount; p++)
			{
				auto param = entryPointReflection->getParameterByIndex(p);
				auto paramName = param->getName();
				auto paramType = param->getType();

				// Only process input parameters for vertex shaders
				if (param == SLANG_PARAMETER_CATEGORY_VERTEX_INPUT &&
					stage == ShaderStage::Vertex)
				{
					// Get semantic information
					auto semanticName = param->getSemanticName();
					uint32_t semanticIndex = param->getSemanticIndex();

					if (semanticName && strstr(semanticName, "ATTRIB") != nullptr)
					{
						ShaderAttribute attribute;
						attribute.Name = paramName ? paramName : "UnnamedAttribute";
						attribute.Location = semanticIndex;
						attribute.Type = SlangTypeToShaderDataType(paramType);

						reflection.AddAttribute(attribute);
					}
				}
			}
		}

		// Extract global parameters (uniforms, textures, etc.)
		uint32_t globalCount = programReflection->getParameterCount();
		for (uint32_t g = 0; g < globalCount; g++)
		{
			auto param = programReflection->getParameterByIndex(g);
			auto paramName = param->getName();
			auto paramType = param->getType();

			// Skip non-resource parameters
			if (!paramType->isResourceType() && !paramType->isBuffer())
				continue;

			// Get binding information
			uint32_t bindingSet = param->getBindingSpace();
			uint32_t bindingIndex = param->getBindingIndex();

			// Handle different resource types
			if (paramType->isResourceType())
			{
				auto bindingType = paramType->getResourceType();
				auto stages = ShaderStage::All; // Default to all stages

				ShaderResourceBinding binding;
				binding.Name = paramName ? paramName : "UnnamedResource";
				binding.Set = bindingSet;
				binding.Binding = bindingIndex;
				binding.Count = 1;
				binding.Stage = stages;
				binding.Type = SlangBindingTypeToDescriptorType(bindingType);

				// Handle array types
				if (paramType->isArray())
				{
					binding.ArraySize = paramType->getElementCount();
					binding.Count = binding.ArraySize;
				}

				reflection.AddResourceBinding(binding);
			}
			else if (paramType->isBuffer())
			{
				// Uniform buffer
				ShaderUniformBuffer ubo;
				ubo.Name = paramName ? paramName : "UnnamedUBO";
				ubo.Set = bindingSet;
				ubo.Binding = bindingIndex;
				ubo.Size = (uint32_t)paramType->getSize();
				ubo.Stage = ShaderStage::All; // Default to all stages

				// Extract struct members if available
				if (paramType->isStruct())
				{
					uint32_t fieldCount = paramType->getFieldCount();
					for (uint32_t f = 0; f < fieldCount; f++)
					{
						auto field = paramType->getFieldByIndex(f);
						auto fieldName = field->getName();
						auto fieldType = field->getType();

						BufferElement element;
						element.Name = fieldName ? fieldName : "UnnamedField";
						element.Type = SlangTypeToShaderDataType(fieldType);
						element.Size = (uint32_t)fieldType->getSize();
						element.Offset = (uint32_t)field->getOffset();

						ubo.Elements.push_back(element);
					}
				}

				reflection.AddUniformBuffer(ubo);
			}
		}

		return reflection;
	}

	ShaderDataType ShaderLibrary::SlangTypeToShaderDataType(slang::TypeReflection* type)
	{
		switch (type->getKind())
		{
			case SLANG_TYPE_KIND_SCALAR:
			{
				auto baseType = type->getScalarType();
				if (baseType == SLANG_SCALAR_TYPE_FLOAT32)
					return ShaderDataType::Float;
				else if (baseType == SLANG_SCALAR_TYPE_INT32)
					return ShaderDataType::Int;
				else if (baseType == SLANG_SCALAR_TYPE_BOOL)
					return ShaderDataType::Bool;

				return ShaderDataType::Float; // Default
			}
			case SLANG_TYPE_KIND_VECTOR:
			{
				auto elementType = type->getElementType();
				auto elementCount = type->getElementCount();

				if (elementType->getScalarType() == SLANG_SCALAR_TYPE_FLOAT32)
				{
					switch (elementCount)
					{
						case 2: return ShaderDataType::Float2;
						case 3: return ShaderDataType::Float3;
						case 4: return ShaderDataType::Float4;
					}
				}
				else if (elementType->getScalarType() == SLANG_SCALAR_TYPE_INT32)
				{
					switch (elementCount)
					{
						case 2: return ShaderDataType::Int2;
						case 3: return ShaderDataType::Int3;
						case 4: return ShaderDataType::Int4;
					}
				}

				return ShaderDataType::Float4; // Default for vectors
			}
			case SLANG_TYPE_KIND_MATRIX:
			{
				auto rowCount = type->getRowCount();
				auto colCount = type->getColumnCount();

				if (rowCount == 3 && colCount == 3)
					return ShaderDataType::Mat3;
				else if (rowCount == 4 && colCount == 4)
					return ShaderDataType::Mat4;

				return ShaderDataType::Mat4; // Default for matrices
			}
			default:
				return ShaderDataType::Float; // Default
		}
	}

	DescriptorType ShaderLibrary::SlangBindingTypeToDescriptorType(slang::BindingType bindingType)
	{
		switch (bindingType)
		{
			case SLANG_BINDING_TYPE_SAMPLER:
				return DescriptorType::SampledImage;
			case SLANG_BINDING_TYPE_TEXTURE:
				return DescriptorType::SampledImage;
			case SLANG_BINDING_TYPE_CONSTANT_BUFFER:
				return DescriptorType::UniformBuffer;
			case SLANG_BINDING_TYPE_PARAMETER_BLOCK:
				return DescriptorType::UniformBuffer;
			case SLANG_BINDING_TYPE_TYPED_BUFFER:
				return DescriptorType::StorageBuffer;
			case SLANG_BINDING_TYPE_RAW_BUFFER:
				return DescriptorType::StorageBuffer;
				case SLANG_BINDING_TYPE_COMBINED_TEXTURE_SAMPLER
					return DescriptorType::SampledImage;
				default:
					return DescriptorType::SampledImage; // Default
		}
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
			default:
				return ShaderStage::All;
		}
	}
}