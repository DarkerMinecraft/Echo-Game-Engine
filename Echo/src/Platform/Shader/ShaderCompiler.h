#pragma once

#include <dxc/dxcapi.h>
#include <dxc/WinAdapter.h>
#include <atlcomcli.h>

#include <vector>

namespace Echo 
{

	enum RendererType 
	{
		Vulkan13,
		DirectX
	};

	enum ShaderType
	{
		VertexShader,
		FragmentShader,
		ComputeShader,
		GeometryShader
	};

	class SpirvData
	{
	public:
		SpirvData(size_t size, uint32_t* data)
			: m_Size(size), m_Data(data)
		{}
		SpirvData() = default;

		size_t GetSize() { return m_Size; }
		uint32_t* GetData() { return m_Data; }
	private:
		size_t m_Size;
		uint32_t* m_Data;
	};

	class ShaderCompiler 
	{
	public:
		ShaderCompiler(const std::wstring& filePath, RendererType rendererType, ShaderType shaderType);

		SpirvData GetSpriv() { return m_Spirv; }
	private:
		void LoadHLSL(const std::wstring& filePath, RendererType rendererType, ShaderType shaderType);
	private:
		SpirvData m_Spirv;
	};

	class ShaderLibrary 
	{
	public:
		SpirvData AddSpirvShader(const std::wstring& filePath, ShaderType type);
		SpirvData GetSpirv(const wchar_t* name) { return m_SpirvMap[name]; }
	private:
		std::map<const wchar_t*, SpirvData> m_SpirvMap;
	};
}