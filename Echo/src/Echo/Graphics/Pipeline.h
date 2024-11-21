#pragma once

#include "Echo/Core/Base.h"
#include <string>

namespace Echo 
{
	enum PipelineType 
	{
		GraphicsPipeline, 
		ComputePipeline
	};

	enum ShaderType 
	{
		Vertex, 
		Fragment, 
		Compute,
		Unknown
	};

	class Pipeline 
	{
	public:
		virtual ~Pipeline() = default;

		virtual PipelineType GetPipelineType() = 0;

		virtual void Bind() = 0;

		static Ref<Pipeline> Create(PipelineType type, const std::string& filePath);
	};
}