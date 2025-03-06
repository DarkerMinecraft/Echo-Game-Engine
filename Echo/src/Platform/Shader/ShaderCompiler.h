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
		ShaderCompiler(const std::string& filePath, RendererType rendererType, ShaderType shaderType);
		ShaderCompiler(const char* shaderSource, RendererType rendererType, ShaderType shaderType);

		SpirvData GetSpriv() { return m_Spirv; }
	private:
		void LoadHLSL(const std::string& filePath, RendererType rendererType, ShaderType shaderType);
		void LoadHLSLFromSource(const char* shaderSource,
								RendererType rendererType, ShaderType shaderType);
	private:
		SpirvData m_Spirv;
	};

	class ShaderLibrary 
	{
	public:
		SpirvData AddSpirvShader(const std::string& filePath, ShaderType type);
		SpirvData AddSpirvShader(const char* shaderSource, std::string shaderName, ShaderType type);

		SpirvData GetSpirv(const char* name) { return m_SpirvMap[name]; }
	private:
		std::map<const char*, SpirvData> m_SpirvMap;
	};
}