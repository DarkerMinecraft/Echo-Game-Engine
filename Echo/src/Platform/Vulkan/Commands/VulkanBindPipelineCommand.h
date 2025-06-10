#pragma once

#include "Graphics/Primitives/CommandBuffer.h"
#include "Graphics/Commands/ICommand.h"

#include "Graphics/Primitives/Pipeline.h"

namespace Echo 
{

	class VulkanBindPipelineCommand : public ICommand
	{
	public:
		VulkanBindPipelineCommand(Pipeline* pipeline)
			: m_Pipeline(pipeline)
		{}

		virtual void Execute(CommandBuffer* cmd) override { m_Pipeline->Bind(cmd); };
	private:
		Pipeline* m_Pipeline;
	};

}