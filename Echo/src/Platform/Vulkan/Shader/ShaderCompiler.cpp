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

	VkShaderModule ShaderLibrary::AddSpirvShader(const std::filesystem::path& path)
	{
		slang::IComponentType* linkedProgram = CompileShader(path);

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
		slang::IComponentType* linkedProgram = CompileShader(source, name);

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

	ShaderReflection ShaderLibrary::ReflectShader(const std::filesystem::path& path)
	{
		slang::IComponentType* linkedProgram = CompileShader(path);

		ShaderReflection reflections = ExtractReflection(linkedProgram);
		return reflections;
	}

	ShaderReflection ShaderLibrary::ReflectShader(const char* source, const char* name)
	{
		slang::IComponentType* linkedProgram = CompileShader(source, name);

		ShaderReflection reflections = ExtractReflection(linkedProgram);
		return reflections;
	}

	ShaderReflection ShaderLibrary::ExtractReflection(slang::IComponentType* program)
	{
		ShaderReflection reflections;

		slang::ProgramLayout* layout; 
		{
			Slang::ComPtr<slang::IBlob> diagnosticsBlob;
			layout = program->getLayout(0i64, diagnosticsBlob.writeRef());
			if (diagnosticsBlob)
			{
				std::string message = (char*)diagnosticsBlob->getBufferPointer();
				EC_CORE_CRITICAL("Failed to get layout: {0}", message);
			}
		}

		uint32_t entryPointCount = layout->getEntryPointCount();

		if (entryPointCount > 1) EC_CORE_CRITICAL("More than 1 entry points");
		slang::EntryPointReflection* entryPoint = layout->getEntryPointByIndex(0);

		ShaderStage stage = SlangStageToShaderStage(entryPoint->getStage());
		switch (stage)
		{
			case ShaderStage::Vertex:
				EC_CORE_INFO("Vertex")
					break;
			case ShaderStage::Fragment:
				EC_CORE_INFO("Fragment")
					break;
			case ShaderStage::Compute:
				EC_CORE_INFO("Compute")
					break;
			case ShaderStage::All:
				EC_CORE_INFO("All")
					break;
		}
		reflections.SetShaderStage(SlangStageToShaderStage(entryPoint->getStage()));

		ExtractVertexAttributes(entryPoint, reflections);

		return reflections;
	}

	void ShaderLibrary::ExtractVertexAttributes(slang::EntryPointReflection* entryPoint, ShaderReflection& reflection)
	{
		if (entryPoint->getStage() != SLANG_STAGE_VERTEX) return;
		EC_CORE_INFO("Getting Vertex Attributes")

			uint32_t paramCount = entryPoint->getParameterCount();
		for (uint32_t paramIndex = 0; paramIndex < paramCount; paramIndex++)
		{
			auto param = entryPoint->getParameterByIndex(paramIndex);

			if (param->getCategory() == slang::ParameterCategory::VaryingInput)
			{
				auto paramType = param->getType();

				if (paramType->getKind() == slang::TypeReflection::Kind::Struct)
				{
					uint32_t fieldCount = paramType->getFieldCount();
					for (uint32_t fieldIndex = 0; fieldIndex < fieldCount; fieldIndex++)
					{
						auto field = paramType->getFieldByIndex(fieldIndex);

						const char* fieldName = field->getName();
						ShaderDataType fieldType = SlangTypeToShaderDataType(field->getType());

						ShaderAttribute attribute;
						attribute.Location = fieldIndex;
						attribute.Name = fieldName;
						attribute.Type = fieldType;

						EC_CORE_INFO("Field Index: {0}\nField Name: {1}", fieldIndex, fieldName)

							reflection.AddAttribute(attribute);
					}
				}
			}
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
				EC_CORE_INFO("Couldn't find the correct shader stage!")
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

	uint32_t ShaderLibrary::ExtractLocationFromSemantic(const char* semantic)
	{
		if (!semantic)
			return 0;

		const char* digitStart = semantic;
		while (*digitStart && !isdigit(*digitStart))
			digitStart++;

		if (*digitStart)
			return atoi(digitStart);

		return 0;
	}

	slang::IComponentType* ShaderLibrary::CompileShader(const std::filesystem::path& path)
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

			slangModule = session->loadModule(path.string().c_str(), diagnosticBlob.writeRef());
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
				EC_CORE_CRITICAL("Failed to find entry point in {0}: ", path.string().c_str());
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

		return linkedProgram.get();
	}

	Slang::ComPtr<IComponentType> ShaderLibrary::CompileShader(const char* source, const char* name)
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

		return linkedProgram;
	}
}