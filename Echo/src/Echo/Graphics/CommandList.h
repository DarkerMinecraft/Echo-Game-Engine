#pragma once 

#include "Echo/Core/Base.h"

#include "Commands/ICommand.h"
#include "CommandBuffer.h"

#include "Image.h"

#include <vector>

namespace Echo
{

	class CommandList 
	{
	public:
		CommandList(); 
		virtual ~CommandList() = default;
		
		void Begin();
		void RecordCommand(Ref<ICommand> command);

		void SetSrcImage(Ref<Image> srcImage) { m_CommandBuffer->SetSourceImage(srcImage); }

		void Execute();
	private:
		std::vector<Ref<ICommand>> m_Commands;
		Ref<CommandBuffer> m_CommandBuffer;
	};

}