#pragma once

#include "Echo/Core/Base.h"

#include <cstdint>
#include <vector>
#include <string>

#include "Texture.h"


namespace Echo 
{

	enum class TextureFormat
	{
		Undefined,
		RGBA8,
		Depth32F,
	};

	enum class TextureUsage
	{
		None = 0,
		Sampled = 1 << 0, 
		ColorAttachment = 1 << 1, 
		DepthAttachment = 1 << 2,  
	};

	inline TextureUsage operator|(TextureUsage a, TextureUsage b)
	{
		return static_cast<TextureUsage>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline bool HasUsage(TextureUsage usage, TextureUsage bit)
	{
		return (static_cast<int>(usage) & static_cast<int>(bit)) != 0;
	}

	struct BufferDesc {};

	struct TextureDesc 
	{
		uint32_t Width = 0;
		uint32_t Height = 0;

		TextureFormat Format = TextureFormat::RGBA8;
		TextureUsage Usage = TextureUsage::Sampled;

		const std::string& TexturePath = "";
		void* Pixels;
	};

	struct PipelineDesc {
		const char* VertexShaderPath = "";
		const char* FragmentShaderPath = "";
		const char* GeometryShaderPath = "";
		const char* ComputeShaderPath = "";
		const char* RayTracingShaderPath = "";

		bool EnableBlending = false;
		float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };  // RGBA blend factor

		bool EnableDepthTest = true;
		bool EnableDepthWrite = true;
		bool EnableCull = true;
		enum class CullMode { None, Front, Back } CullMode = CullMode::Back;
		enum class FillMode { Solid, Wireframe } FillMode = FillMode::Solid;

		enum class CompareOp { Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always } DepthCompareOp = CompareOp::Less;

		struct VertexAttribute
		{
			std::string Name;
			uint32_t Location = 0;
			uint32_t Binding = 0;
			uint32_t Stride = 0;
			enum class Format { Float, Float2, Float3, Float4, Int, Int2, Int3, Int4 } VertexFormat = Format::Float3;
		};
		std::vector<VertexAttribute> VertexAttributes;

		enum class Topology { TriangleList, TriangleStrip, LineList, LineStrip, PointList } GraphicsTopology = Topology::TriangleList;

		struct PushConstant
		{
			uint32_t Size = 0;
			uint32_t Offset = 0;
		};
		PushConstant PushConstants;

		struct DescriptorSetLayout
		{
			uint32_t Binding;
			enum class DescriptorType { UniformBuffer, StorageBuffer, SampledImage, StorageImage } Type;
			uint32_t Count = 1;
			enum class ShaderStage { Vertex, Fragment, Compute, All } Stage;
		};
		std::vector<DescriptorSetLayout> DescriptorSetLayouts;
		uint32_t MaxSets;
	};

	struct FrameBufferDesc 
	{
		bool ClearOnBegin = true;
		bool UseSwapchain;
		uint32_t Width, Height;

		std::vector<Ref<Texture>> ColorAttachments;
		Ref<Texture> DepthAttachment;
	};
}