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

}