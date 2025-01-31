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
		BGRA8,
		Depth32F,
		RGBA16
	};

	enum class TextureUsage
	{
		None = 0,
		Sampled = 1 << 0,
		ColorAttachment = 1 << 1,
		DepthAttachment = 1 << 2,
		TransferSrc = 1 << 3,
		TransferDst = 1 << 4,
		Storage = 1 << 5
	};

	inline TextureUsage operator|(TextureUsage a, TextureUsage b)
	{
		return static_cast<TextureUsage>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline bool HasUsage(TextureUsage usage, TextureUsage bit)
	{
		return (static_cast<int>(usage) & static_cast<int>(bit)) != 0;
	}

	enum BufferUsage
	{
		BUFFER_USAGE_VERTEX = 1 << 0,
		BUFFER_USAGE_INDEX = 1 << 1,
		BUFFER_USAGE_UNIFORM = 1 << 2,
		BUFFER_USAGE_STORAGE = 1 << 3,
		BUFFER_USAGE_TRANSFER_SRC = 1 << 4,
		BUFFER_USAGE_TRANSFER_DST = 1 << 5
	};

	enum MemoryFlags
	{
		MEMORY_HOST_VISIBLE = 1 << 0,
		MEMORY_DEVICE_LOCAL = 1 << 1,
		MEMORY_HOST_COHERENT = 1 << 2
	};

	struct BufferDesc 
	{
		size_t BufferSize;
		uint32_t Usage;
		uint32_t Flags;
		void* InitialData;
		bool UseStagingBuffer;
	};

	struct TextureDesc 
	{
		uint32_t Width = 0;
		uint32_t Height = 0;

		bool UseSwapchainExtent = false;

		TextureFormat Format = TextureFormat::RGBA8;
		TextureUsage Usage = TextureUsage::Sampled;

		const std::string& TexturePath = "";
		void* Pixels;
	};

	enum class VertexFormat { Float, Float2, Float3, Float4, Int, Int2, Int3, Int4 };
	enum class Topology { TriangleList, TriangleStrip, LineList, LineStrip, PointList };
	enum class DescriptorType { UniformBuffer, StorageBuffer, SampledImage, StorageImage };
	enum class ShaderStage { Vertex, Fragment, Compute, All };
	enum class Cull { None, Front, Back };
	enum class Fill { Solid, Wireframe };
	enum class CompareOp { Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always };

	struct PipelineDesc {
		const wchar_t* VertexShaderPath = nullptr;
		const wchar_t* FragmentShaderPath = nullptr;
		const wchar_t* GeometryShaderPath = nullptr;
		const wchar_t* ComputeShaderPath = nullptr;
		const wchar_t* RayTracingShaderPath = nullptr;

		bool EnableBlending = false;
		float BlendFactor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };  

		bool EnableDepthTest = true;
		bool EnableDepthWrite = true;
		bool EnableCull = true;
		Cull CullMode = Cull::Back;
		Fill FillMode = Fill::Solid;

		CompareOp DepthCompareOp = CompareOp::Less;

		struct VertexAttribute
		{
			std::string Name;
			uint32_t Location = 0;
			uint32_t Binding = 0;
			uint32_t Stride = 0;
			VertexFormat Format = VertexFormat::Float3;
		};
		std::vector<VertexAttribute> VertexAttributes;

		Topology GraphicsTopology = Topology::TriangleList;

		struct DescriptorSetLayout
		{
			uint32_t Binding;
			DescriptorType Type;
			uint32_t Count = 1;
			ShaderStage Stage;
		};
		std::vector<DescriptorSetLayout> DescriptorSetLayouts;
		uint32_t MaxSets;
	};

	struct FrameBufferDesc 
	{
		bool ClearOnBegin = true;
		bool UseDrawImage = true;
		bool UseSwapchainImage = false;
		bool UseColorAttachmentSwapchain = false;
		bool UseSwapchainExtent = false;
		uint32_t Width, Height;

		std::vector<Ref<Texture>> ColorAttachments;
		Ref<Texture> DepthAttachment = nullptr;
	};
}