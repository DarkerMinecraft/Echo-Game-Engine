#pragma once

#include "Core/Log.h"

#include "Framebuffer.h"

namespace Echo 
{

	enum class VertexFormat { Float, Float2, Float3, Float4, Int, Int2, Int3, Int4 };
	enum class Topology { TriangleList, TriangleStrip, LineList, LineStrip, PointList };
	enum class DescriptorType { UniformBuffer, StorageBuffer, SampledImage, StorageImage };
	enum class ShaderStage { Vertex, Fragment, Compute, Geometry, All };
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
		// Default constructor for an empty layout
		BufferLayout() = default;

		// Constructor using initializer list (original behavior)
		BufferLayout(const std::initializer_list<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		// NEW: Constructor using a vector of BufferElements
		BufferLayout(const std::vector<BufferElement>& elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		// Getters
		uint32_t GetStride() const { return m_Stride; }
		const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		// Element access via iterators
		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

		// NEW: Method to add a single element to the layout
		void AddElement(const BufferElement& element)
		{
			m_Elements.push_back(element);
			CalculateOffsetsAndStride();
		}

		// NEW: Method to add a single element to the layout by parameters
		void AddElement(ShaderDataType type, const std::string& name, bool normalized = false)
		{
			m_Elements.emplace_back(type, name, normalized);
			CalculateOffsetsAndStride();
		}

		// NEW: Method to add multiple elements from a vector
		void AddElements(const std::vector<BufferElement>& elements)
		{
			m_Elements.insert(m_Elements.end(), elements.begin(), elements.end());
			CalculateOffsetsAndStride();
		}

		// NEW: Method to check if layout is empty
		bool IsEmpty() const { return m_Elements.empty(); }

		// NEW: Method to get the number of elements
		size_t Size() const { return m_Elements.size(); }
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

	struct DescriptionSetLayout
	{
		uint32_t Binding;
		uint32_t Set;
		DescriptorType Type;
		uint32_t Count;
		ShaderStage Stage;
	};

	struct PipelineSpecification
	{
		bool EnableBlending = false;
		float BlendFactor[4] = { 1.0, 1.0f, 1.0f, 1.0f };

		bool EnableDepthTest = false;
		bool EnableDepthWrite = false;
		bool EnableCulling = true;
		Cull CullMode = Cull::Front;
		Fill FillMode = Fill::Solid;

		CompareOp DepthCompareOp = CompareOp::Less;
		Topology GraphicsTopology = Topology::TriangleList;

		Ref<Framebuffer> RenderTarget;
	};

	enum TextureFilter
	{
		Linear = 0,
		Nearest
	};

	struct Texture2DSpecification 
	{
		TextureFilter MinFilter = Nearest, MagFilter = Nearest;
	};

}