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

	void ShaderLibrary::AddSpirvShader(const std::filesystem::path& path, VkShaderModule* module, ShaderReflection* reflection)
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

		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, module) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create shader module!");
		}

		slang::ProgramLayout* layout = linkedProgram->getLayout();
		ExtractReflection(layout, reflection);
	}

	void ShaderLibrary::AddSpirvShader(const char* source, const char* name, VkShaderModule* module, ShaderReflection* reflection)
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

		if (vkCreateShaderModule(m_Device, &createInfo, nullptr, module) != VK_SUCCESS)
		{
			EC_CORE_CRITICAL("Failed to create shader module!");
		}

		slang::ProgramLayout* layout = linkedProgram->getLayout();
		ExtractReflection(layout, reflection);
	}

	void ShaderLibrary::ExtractReflection(slang::ProgramLayout* layout, ShaderReflection* reflection)
	{
		uint32_t entryPointCount = layout->getEntryPointCount();

		if (entryPointCount > 1) EC_CORE_CRITICAL("More than 1 entry points");
		slang::EntryPointReflection* entryPoint = layout->getEntryPointByIndex(0);

		reflection->SetShaderStage(SlangStageToShaderStage(entryPoint->getStage()));

		ExtractVertexAttributes(entryPoint, reflection);
	}

	void ShaderLibrary::ExtractVertexAttributes(slang::EntryPointReflection* entryPoint, ShaderReflection* reflection)
	{
		if (entryPoint->getStage() != SLANG_STAGE_VERTEX) return;

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

						reflection->AddAttribute(attribute);
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
			case SLANG_STAGE_GEOMETRY:
				return ShaderStage::Geometry;
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
}