#pragma once

#include "Pipeline.h"

namespace Echo
{
	class Material
	{
	public:
		Material(Ref<Pipeline> pipeline)
			: m_Pipeline(pipeline)
		{}
		virtual ~Material() = default;

		Ref<Pipeline> GetPipeline() { return m_Pipeline; }
	private:
		Ref<Pipeline> m_Pipeline;
	};
}