#pragma once

#include "Echo/Core/Application.h"

#include "Device.h"
#include "Buffer.h"
#include "Texture.h"
#include "Pipeline.h"
#include "FrameBuffer.h"

#include "RHIDesc.h"

namespace Echo 
{

	class RHI 
	{
	public:
		static inline Ref<Buffer> CreateBuffer(const BufferDesc& bufferDescription) 
		{
			return GetDevice()->CreateBuffer(bufferDescription);
		}

		static inline Ref<Texture> CreateTexture(const TextureDesc& textureDescription) 
		{
			return GetDevice()->CreateTexture(textureDescription);
		}

		static inline Ref<Pipeline> CreatePipeline(const PipelineDesc& pipelineDescription)
		{
			return GetDevice()->CreatePipeline(pipelineDescription);
		}

		static inline Ref<FrameBuffer> CreateFrameBuffer(const FrameBufferDesc& frameBufferDescription) 
		{
			return GetDevice()->CreateFrameBuffer(frameBufferDescription);
		}

		static inline void CMDDispatch(float groupXScale, float groupYScale) 
		{
			GetDevice()->CMDDispatch(groupXScale, groupYScale);
		}

	private:
		static inline Device* GetDevice() 
		{
			return Application::Get().GetWindow().GetDevice();
		}
	};

}