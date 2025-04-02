#pragma once

#include "Graphics/CommandBuffer.h"
#include "Graphics/Commands/ICommand.h"

#include "Graphics/Pipeline.h"

namespace Echo 
{

	class VulkanBindPipelineCommand : public ICommand
	{
	public:
		VulkanBindPipelineCommand(Ref<Pipeline> pipeline)
			: m_Pipeline(pipeline)
		{}

		virtual void Execute(CommandBuffer* cmd) override { m_Pipeline->Bind(cmd); };
	private:
		Ref<Pipeline> m_Pipeline;
	};

}