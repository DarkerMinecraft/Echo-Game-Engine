#include "pch.h"
#include "ShaderCompiler.h"

#include "Vulkan/VulkanRenderCaps.h"

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

	std::vector<VkShaderModule> ShaderLibrary::AddSpirvShader(const std::filesystem::path& path, ShaderReflection* reflection)
	{
		std::vector<VkShaderModule> modules;

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
			}
		}

		uint32_t entryPointCount = slangModule->getDefinedEntryPointCount();
		for (int i = 0; i < entryPointCount; i++)
		{
			IEntryPoint* entryPoint;
			{
				slangModule->getDefinedEntryPoint(i, &entryPoint);
				if (!entryPoint)
				{
					EC_CORE_CRITICAL("Failed to find entry point in {0}: ", path.string().c_str());
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

			EC_CORE_INFO("SPIR-V code size: {0} bytes", spirvCode->getBufferSize());
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

			slang::ProgramLayout* layout = linkedProgram->getLayout();
			slang::EntryPointReflection* entryPointReflection = layout->getEntryPointByIndex(0);
			ShaderStage shaderStage = SlangStageToShaderStage(entryPointReflection->getStage());

			EC_CORE_INFO("Shader Reflection {0}", entryPointReflection->getName());

			reflection->AddEntryPointData({ shaderStage, entryPointReflection->getName() });

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

		return modules;
	}

	void ShaderLibrary::ExtractVertexAttributes(slang::EntryPointReflection* entryPoint, ShaderReflection* reflection)
	{
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
					}
				}
			}
		}

		reflection->SetBufferLayout(layout);
	}

	void ShaderLibrary::ExtractBuffers(ShaderStage stage, slang::ProgramLayout* layout, IMetadata* metadata, ShaderReflection* reflection)
	{
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

			EC_CORE_INFO("Name {0}, Binding {1}, Set {2}, Stage {3}", name, binding, set, (uint32_t)stage)

			if (type->getKind() == slang::TypeReflection::Kind::ConstantBuffer)
			{
				ShaderResourceBinding ubo{};
				ubo.Name = name;
				ubo.Binding = binding;
				ubo.Set = set;
				ubo.Stage = stage;
				ubo.Type = DescriptorType::UniformBuffer;
				reflection->AddResourceBinding(ubo);
			}
			else
			{
				ShaderResourceBinding res = {};
				res.Name = name;
				res.Set = set;
				res.Binding = binding;
				res.Stage = stage;

				auto shape = type->getResourceShape();
				auto access = type->getResourceAccess();

				if (shape == SlangResourceShape::SLANG_TEXTURE_2D)
					res.Type = (access == SLANG_RESOURCE_ACCESS_READ)
					? DescriptorType::SampledImage
					: DescriptorType::StorageImage;
				else if (shape == SlangResourceShape::SLANG_STRUCTURED_BUFFER)
					res.Type = DescriptorType::StorageBuffer;

				res.Count = type->isArray()
					? type->getTotalArrayElementCount() == 0 ? VulkanRenderCaps::GetMaxTextureSlots() : type->getTotalArrayElementCount()
					: 1;

				reflection->AddResourceBinding(res);
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