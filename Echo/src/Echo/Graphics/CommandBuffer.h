#pragma once 

#include "Framebuffer.h"

namespace Echo 
{

	class CommandBuffer
	{
	public:
		virtual	~CommandBuffer() = default;

		virtual void Start() = 0;
		virtual void End() = 0;

		virtual void Submit() = 0;

		virtual void SetSourceFramebuffer(Ref<Framebuffer> framebuffer) = 0;
		virtual void SetShouldPresent(bool shouldPresent) = 0;
		virtual void SetDrawToSwapchain(bool drawToSwapchain) = 0;
		
		static Ref<CommandBuffer> Create();
	};

}