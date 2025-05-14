#pragma once

#include "Framebuffer.h"

namespace Echo 
{

	enum class VertexFormat { Float, Float2, Float3, Float4, Int, Int2, Int3, Int4 };
	enum class Topology { TriangleList, TriangleStrip, LineList, LineStrip, PointList };
	enum class DescriptorType { UniformBuffer, StorageBuffer, SampledImage, StorageImage };
	enum class ShaderStage { Vertex, Fragment, Compute, All };
	enum class Cull { None, Front, Back };
	enum class Fill { Solid, Wireframe };
	enum class CompareOp { Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always };

	enum class ShaderDataType
	{
		None = 0,
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		Mat3, Mat4,
		Bool
	};

	static uint32_t ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case ShaderDataType::Float:     return 4;
			case ShaderDataType::Float2:    return 4 * 2;
			case ShaderDataType::Float3:    return 4 * 3;
			case ShaderDataType::Float4:    return 4 * 4;
			case ShaderDataType::Int:       return 4;
			case ShaderDataType::Int2:      return 4 * 2;
			case ShaderDataType::Int3:      return 4 * 3;
			case ShaderDataType::Int4:      return 4 * 4;
			case ShaderDataType::Mat3:      return 4 * 3 * 3;
			case ShaderDataType::Mat4:      return 4 * 4 * 4;
			case ShaderDataType::Bool:      return 1;
			default: return 0;
		}
	}

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		uint32_t Offset;
		bool Normalized;
		BufferElement() = default;
		BufferElement(ShaderDataType type, const std::string& name, bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized)
		{}

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:  return 1;
				case ShaderDataType::Float2: return 2;
				case ShaderDataType::Float3: return 3;
				case ShaderDataType::Float4: return 4;
				case ShaderDataType::Int:    return 1;
				case ShaderDataType::Int2:   return 2;
				case ShaderDataType::Int3:   return 3;
				case ShaderDataType::Int4:   return 4;
				case ShaderDataType::Mat3:   return 3 * 3;
				case ShaderDataType::Mat4:   return 4 * 4;
				case ShaderDataType::Bool:   return 1;
			}
			EC_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;

		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

	private:
		void CalculateOffsetsAndStride()
		{
			uint32_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

	struct PipelineSpecification
	{
		bool EnableBlending = false;
		float BlendFactor[4] = { 1.0, 1.0f, 1.0f, 1.0f };

		bool EnableDepthTest = false;
		bool EnableDepthWrite = false;
		bool EnableCulling = true;
		Cull CullMode = Cull::Back;
		Fill FillMode = Fill::Solid;

		CompareOp DepthCompareOp = CompareOp::Less;
		BufferLayout VertexLayout;

		Topology GraphicsTopology = Topology::TriangleList;

		struct DescriptionSetLayout
		{
			uint32_t Binding = 0;
			DescriptorType Type;
			uint32_t Count;
			ShaderStage Stage;
		};
		std::vector<DescriptionSetLayout> DescriptionSetLayouts;

		Ref<Framebuffer> RenderTarget;
	};

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

}