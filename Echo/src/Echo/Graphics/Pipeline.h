#pragma once

#include "Echo/Core/Base.h"
#include "Material.h"

#include <vector>
#include "Image.h"
#include "CommandBuffer.h"

namespace Echo 
{

	enum PipelineType { Graphics, Compute };

	enum class VertexFormat { Float, Float2, Float3, Float4, Int, Int2, Int3, Int4 };
	enum class Topology { TriangleList, TriangleStrip, LineList, LineStrip, PointList };
	enum class DescriptorType { UniformBuffer, StorageBuffer, SampledImage, StorageImage };
	enum class ShaderStage { Vertex, Fragment, Compute, All };
	enum class Cull { None, Front, Back };
	enum class Fill { Solid, Wireframe };
	enum class CompareOp { Never, Less, Equal, LessOrEqual, Greater, NotEqual, GreaterOrEqual, Always };

	struct PipelineDesc 
	{
		bool EnableBlending = false;
		float BlendFactor[4] = { 1.0, 1.0f, 1.0f, 1.0f };

		bool EnableDepthTest = false;
		bool EnableDepthWrite = false;
		bool EnableCulling = true;
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
			uint32_t Offset = 0;
		};
		std::vector<VertexAttribute> VertexAttributes;

		Topology GraphicsTopology = Topology::TriangleList;

		struct DescriptionSetLayout 
		{
			uint32_t Binding = 0;
			DescriptorType Type;
			uint32_t Count; 
			ShaderStage Stage;
		};
		std::vector<DescriptionSetLayout> DescriptionSetLayouts;
		uint32_t MaxSets;

		Ref<Image> RenderTarget;
	};

	class Pipeline 
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind(CommandBuffer* cmd) = 0;

		virtual void WriteDesciptorStorageImage(Ref<Image> image, uint32_t binding) = 0;

		static Ref<Pipeline> Create(Ref<Material> material, PipelineDesc& desc);
		static Ref<Pipeline> Create(const char* computeFilePath, PipelineDesc& desc);
	};

}