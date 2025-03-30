#pragma once 

namespace Echo 
{

	struct ShaderSpecification
	{
		const char* VertexShaderPath = nullptr;
		const char* FragmentShaderPath = nullptr;
		const char* GeometryShaderPath = nullptr;
		const char* ComputeShaderPath = nullptr;

		const char* VertexShaderSource = nullptr;
		const char* FragmentShaderSource = nullptr;
		const char* GeometryShaderSource = nullptr;
		const char* ComputeShaderSource = nullptr;
		const char* ShaderName = nullptr;
	};

	class Shader 
	{
	public:
		virtual ~Shader() = default;

		virtual void Destroy() = 0;

		static Ref<Shader> Create(const ShaderSpecification& specification);
	};

}