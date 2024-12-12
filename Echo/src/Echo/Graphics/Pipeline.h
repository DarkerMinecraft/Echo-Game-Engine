#pragma once

#include "Echo/Core/Base.h"

#include "Descriptors.h"

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace Echo 
{
	enum PipelineType 
	{
		GraphicsPipeline, 
		ComputePipeline
	};

	enum ShaderType 
	{
		VertexShader, 
		FragmentShader, 
		ComputeShader,
	};

	class Pipeline 
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineType GetPipelineType() = 0;
		virtual void* GetPipelineLayout() = 0;
		virtual void* GetDescriptorLayout() = 0;

		virtual void Bind() = 0;
		virtual void UpdatePushConstants(const void* pushConstants) = 0;

		static Ref<Pipeline> Create(PipelineType type, const std::string& filePath, size_t pushConstantsSize = -1, std::vector<DescriptorType> types = {});
	};
}