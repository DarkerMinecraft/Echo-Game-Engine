#include "pch.h"
#include "CommandList.h"

namespace Echo 
{

	CommandList::CommandList()
		: m_CommandBuffer(CommandBuffer::Create())
	{

	}

	void CommandList::Begin() 
	{
		m_Commands.clear();
		m_CommandBuffer->Start();
	}

	void CommandList::RecordCommand(Ref<ICommand> command) 
	{
		m_Commands.push_back(command);
	}

	void CommandList::Execute() 
	{
		if (!m_CommandBuffer->CanExecuteCommands()) return;

		for (Ref<ICommand> command : m_Commands)
		{
			command->Execute(m_CommandBuffer.get());
		}

		m_CommandBuffer->End();
		m_CommandBuffer->Submit();
	}

}