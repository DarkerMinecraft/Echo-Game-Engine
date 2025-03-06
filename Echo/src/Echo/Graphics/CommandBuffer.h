#pragma once 

#include "Image.h"

namespace Echo 
{

	class CommandBuffer
	{
	public:
		virtual	~CommandBuffer() = default;

		virtual void Start() = 0;
		virtual void End() = 0;

		virtual void Submit() = 0;
		virtual void Reset() = 0;

		virtual bool CanExecuteCommands() = 0;

		virtual void SetSourceImage(Ref<Image> srcImage) = 0;

		static Scope<CommandBuffer> Create();
	};

}