#pragma once

#include "Echo/Core/Base.h"

#include "RHIDesc.h"

#include "Buffer.h"
#include "Texture.h"
#include "Pipeline.h"
#include "FrameBuffer.h"

namespace Echo 
{

	enum GraphicsAPI 
	{
		Vulkan, DirectX12
	};

	constexpr unsigned int FRAME_OVERLAP = 2;

	class Device 
	{
	public:
		virtual ~Device() = default;

		virtual void Start() = 0;
		virtual void End() = 0;
		
		virtual Ref<Buffer> CreateBuffer(const BufferDesc& bufferDescription) = 0;
		virtual Ref<Texture> CreateTexture(const TextureDesc& textureDescription) = 0;
		virtual Ref<Pipeline> CreatePipeline(const PipelineDesc& pipleineDescription) = 0;
		virtual Ref<FrameBuffer> CreateFrameBuffer(const FrameBufferDesc& frameBufferDescription) = 0;

		virtual void CMDDispatch(float groupXScale, float groupYScale) = 0;
		
		static Scope<Device> Create(GraphicsAPI api, void* windowHwnd, int width, int height);
	};

}