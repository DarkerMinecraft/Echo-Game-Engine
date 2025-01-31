#pragma once

#include "Buffer.h"
#include "FrameBuffer.h"

namespace Echo 
{
	
	class Pipeline 
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind() = 0;

		virtual void WriteDescriptorBuffer(Ref<Buffer> buffer, uint32_t bufferSize, uint32_t binding) = 0;
		virtual void WriteDescriptorStorageImage(Ref<FrameBuffer> frameBuffer, uint32_t binding) = 0;
	};

}