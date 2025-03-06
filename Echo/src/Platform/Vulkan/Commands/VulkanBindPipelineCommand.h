#pragma once

#include "Echo/Graphics/CommandBuffer.h"
#include "Echo/Graphics/Commands/ICommand.h"

#include "Echo/Graphics/Pipeline.h"

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