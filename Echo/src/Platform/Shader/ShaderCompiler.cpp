#include "pch.h"
#include "ShaderCompiler.h"

#include "Echo/Core/Base.h"

#include <atlstr.h>
#include <strsafe.h>

namespace Echo 
{
	using namespace ATL;

	CStringW ConvertUTF8ToUTF16(__in const CHAR* pszTextUTF8)
	{
		//
		// Special case of NULL or empty input string
		//
		if ((pszTextUTF8 == NULL) || (*pszTextUTF8 == '\0'))
		{
			// Return empty string
			return L"";
		}


		//
		// Consider CHAR's count corresponding to total input string length,
		// including end-of-string (\0) character
		//
		const size_t cchUTF8Max = INT_MAX - 1;
		size_t cchUTF8;
		HRESULT hr = ::StringCchLengthA(pszTextUTF8, cchUTF8Max, &cchUTF8);
		if (FAILED(hr))
		{
			AtlThrow(hr);
		}

		// Consider also terminating \0
		++cchUTF8;

		// Convert to 'int' for use with MultiByteToWideChar API
		int cbUTF8 = static_cast<int>(cchUTF8);


		//
		// Get size of destination UTF-16 buffer, in WCHAR's
		//
		int cchUTF16 = ::MultiByteToWideChar(
			CP_UTF8,                // convert from UTF-8
			MB_ERR_INVALID_CHARS,   // error on invalid chars
			pszTextUTF8,            // source UTF-8 string
			cbUTF8,                 // total length of source UTF-8 string,
									// in CHAR's (= bytes), including end-of-string \0
			NULL,                   // unused - no conversion done in this step
			0                       // request size of destination buffer, in WCHAR's
		);
		ATLASSERT(cchUTF16 != 0);
		if (cchUTF16 == 0)
		{
			AtlThrowLastWin32();
		}


		//
		// Allocate destination buffer to store UTF-16 string
		//
		CStringW strUTF16;
		WCHAR* pszUTF16 = strUTF16.GetBuffer(cchUTF16);

		//
		// Do the conversion from UTF-8 to UTF-16
		//
		int result = ::MultiByteToWideChar(
			CP_UTF8,                // convert from UTF-8
			MB_ERR_INVALID_CHARS,   // error on invalid chars
			pszTextUTF8,            // source UTF-8 string
			cbUTF8,                 // total length of source UTF-8 string,
									// in CHAR's (= bytes), including end-of-string \0
			pszUTF16,               // destination buffer
			cchUTF16                // size of destination buffer, in WCHAR's
		);
		ATLASSERT(result != 0);
		if (result == 0)
		{
			AtlThrowLastWin32();
		}

		// Release internal CString buffer
		strUTF16.ReleaseBuffer();

		// Return resulting UTF16 string
		return strUTF16;
	}

	ShaderCompiler::ShaderCompiler(const std::string& filePath, RendererType rendererType, ShaderType shaderType)
	{
		LoadHLSL(filePath, rendererType, shaderType);
	}

	ShaderCompiler::ShaderCompiler(const char* shaderSource, RendererType rendererType, ShaderType shaderType)
	{
		LoadHLSLFromSource(shaderSource, rendererType, shaderType);
	}

	void ShaderCompiler::LoadHLSL(const std::string& filePath, RendererType rendererType, ShaderType shaderType)
	{
		const wchar_t* file = ConvertUTF8ToUTF16(filePath.c_str());

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
		hres = utils->LoadFile(file, &codePage, &sourceBlob);
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

	void ShaderCompiler::LoadHLSLFromSource(const char* shaderSource,
											RendererType rendererType, ShaderType shaderType)
	{
		const wchar_t* source = ConvertUTF8ToUTF16(shaderSource);

		HRESULT hres;

		// Create the DXC compiler and utility instances.
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

		size_t len = wcslen(source);
		uint32_t byteSize = static_cast<uint32_t>((len + 1) * sizeof(wchar_t));

		CComPtr<IDxcBlobEncoding> sourceBlob;
		hres = utils->CreateBlobFromPinned(
			reinterpret_cast<LPBYTE>(const_cast<wchar_t*>(source)),
			byteSize,
			1200,
			&sourceBlob
		);

		if (FAILED(hres))
		{
			throw std::runtime_error("Could not create blob from shader source");
		}

		// Select the target profile based on shader type.
		LPCWSTR targetProfile = nullptr;
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
			default:
				// You can choose a default or throw an error.
				targetProfile = L"ps_6_6";
				break;
		}

		// Setup compile arguments.
		std::vector<LPCWSTR> arguments = {
			L"-E", L"main",
			L"-T", targetProfile,
			L"-spirv",
			L"-Zi"
		};

		// Fill in the DxcBuffer with our source data.
		DxcBuffer buffer{};
		buffer.Ptr = sourceBlob->GetBufferPointer();
		buffer.Size = sourceBlob->GetBufferSize();
		buffer.Encoding = CP_UTF8;

		// Compile the shader.
		CComPtr<IDxcResult> result{ nullptr };
		hres = compiler->Compile(
			&buffer,
			arguments.data(),
			static_cast<uint32_t>(arguments.size()),
			nullptr,
			IID_PPV_ARGS(&result)
		);

		if (SUCCEEDED(hres))
		{
			result->GetStatus(&hres);
		}

		// If compilation failed, retrieve and log the error message.
		if (FAILED(hres) && result)
		{
			CComPtr<IDxcBlobEncoding> errorBlob;
			hres = result->GetErrorBuffer(&errorBlob);
			if (SUCCEEDED(hres) && errorBlob)
			{
				EC_CORE_ERROR("Shader compilation failed:\n{0}", (const char*)errorBlob->GetBufferPointer());
				throw std::runtime_error("Compilation failed");
			}
		}

		// Get the compiled SPIR-V code.
		CComPtr<IDxcBlob> code;
		result->GetResult(&code);

		m_Spirv = SpirvData(code->GetBufferSize(), reinterpret_cast<uint32_t*>(code->GetBufferPointer()));
	}


	SpirvData ShaderLibrary::AddSpirvShader(const std::string& filePath, ShaderType type)
	{
		size_t lastSlash = filePath.find_last_of("/\\");
		std::string fileName = (lastSlash == std::wstring::npos) ? filePath : filePath.substr(lastSlash + 1);

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

	SpirvData ShaderLibrary::AddSpirvShader(const char* shaderSource, std::string shaderName, ShaderType type)
	{
		if (m_SpirvMap.contains(shaderName.c_str())) return GetSpirv(shaderName.c_str());

		ShaderCompiler compiler(shaderSource, RendererType::Vulkan13, type);
		SpirvData spirv = compiler.GetSpriv();
		m_SpirvMap[shaderName.c_str()] = spirv;
		return spirv;
	}

}