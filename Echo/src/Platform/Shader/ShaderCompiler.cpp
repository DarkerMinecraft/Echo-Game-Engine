#include "pch.h"
#include "ShaderCompiler.h"

#include "Echo/Core/Base.h"

namespace Echo 
{

	ShaderCompiler::ShaderCompiler(const std::wstring& filePath, RendererType rendererType, ShaderType shaderType)
	{
		LoadHLSL(filePath, rendererType, shaderType);
	}

	void ShaderCompiler::LoadHLSL(const std::wstring& filePath, RendererType rendererType, ShaderType shaderType)
	{
		HRESULT hres;

		CComPtr<IDxcCompiler3> compiler;
		hres = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
		if (FAILED(hres))
		{
			throw std::runtime_error("Could not init DXC Compiler");
		}

		CComPtr<IDxcUtils> utils;
		hres = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
		if (FAILED(hres))
		{
			throw std::runtime_error("Could not init DXC Utility");
		}

		uint32_t codePage = DXC_CP_ACP;
		CComPtr<IDxcBlobEncoding> sourceBlob;
		hres = utils->LoadFile(filePath.c_str(), &codePage, &sourceBlob);
		if (FAILED(hres))
		{
			throw std::runtime_error("Could not load shader file");
		}

		LPCWSTR targetProfile{};
		switch (shaderType)
		{
			case VertexShader:
				 targetProfile = L"vs_6_6";  
				 break;
			case FragmentShader:
				targetProfile = L"ps_6_6"; 
				break;
			case ComputeShader:
				targetProfile = L"cs_6_6"; 
				break;
			case GeometryShader:
				targetProfile = L"gs_6_6";  
				break;
		}

		std::vector<LPCWSTR> arguments = {
			L"-E", L"main",
			L"-T", targetProfile,
			L"-spirv",
			L"-Zi"
		};


		DxcBuffer buffer{};
		buffer.Encoding = DXC_CP_ACP;
		buffer.Ptr = sourceBlob->GetBufferPointer();
		buffer.Size = sourceBlob->GetBufferSize();

		CComPtr<IDxcResult> result{ nullptr };
		hres = compiler->Compile(
			&buffer,
			arguments.data(),
			(uint32_t)arguments.size(),
			nullptr,
			IID_PPV_ARGS(&result)
		);

		if (SUCCEEDED(hres))
		{
			result->GetStatus(&hres);
		}

		if (FAILED(hres) && (result))
		{
			CComPtr<IDxcBlobEncoding> errorBlob;
			hres = result->GetErrorBuffer(&errorBlob);
			if (SUCCEEDED(hres) && errorBlob)
			{
				EC_CORE_ERROR("Shader compilation failed:\n{0}", (const char*)errorBlob->GetBufferPointer());
				throw std::runtime_error("Compilation failed");
			}
		}

		CComPtr<IDxcBlob> code;
		result->GetResult(&code);

		m_Spirv = SpirvData(code->GetBufferSize(), (uint32_t*) code->GetBufferPointer());
	}

	SpirvData ShaderLibrary::AddSpirvShader(const std::wstring& filePath, ShaderType type)
	{
		size_t lastSlash = filePath.find_last_of(L"/\\");
		std::wstring fileName = (lastSlash == std::wstring::npos) ? filePath : filePath.substr(lastSlash + 1);

		size_t lastDot = fileName.find_last_of(L'.');
		if (lastDot != std::wstring::npos)
		{
			fileName = fileName.substr(0, lastDot);
		}

		if (m_SpirvMap.contains(fileName.c_str())) return GetSpirv(fileName.c_str());
		
		ShaderCompiler compiler(filePath, RendererType::Vulkan13, type);
		SpirvData spirv = compiler.GetSpriv();
		m_SpirvMap[fileName.c_str()] = spirv;
		return spirv;
		
	}

}