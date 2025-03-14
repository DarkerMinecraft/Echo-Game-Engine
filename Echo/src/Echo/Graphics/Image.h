#pragma once

#include "Echo/Core/Base.h"

#include <glm/glm.hpp>

namespace Echo
{

	typedef enum ImageFlags
	{
		ColorAttachmentBit = 0x01,        // 1 << 0
		TransferSrcBit = 0x02,            // 1 << 1
		TransferDstBit = 0x04,            // 1 << 2
		SampledBit = 0x08,                // 1 << 3
		StorageBit = 0x10,                // 1 << 4
		DepthStencilAttachmentBit = 0x20  // 1 << 5
	} ImageFlags;


	inline ImageFlags operator|(ImageFlags a, ImageFlags b)
	{
		return static_cast<ImageFlags>(static_cast<int>(a) | static_cast<int>(b));
	}

	inline ImageFlags operator&(ImageFlags a, ImageFlags b)
	{
		return static_cast<ImageFlags>(static_cast<int>(a) & static_cast<int>(b));
	}

	inline ImageFlags& operator|=(ImageFlags& a, ImageFlags b)
	{
		return a = a | b;
	}

	enum ImageFormat
	{
		RGBA8,
		RGBA16F,
		RGBA32F,
		R32F,
		RG32F,
		RGB32F,
		RGBA16,
		RGBA8Srgb,
		BGRA8,
		Depth32F,
		Depth24Stencil8
	};

	struct ImageDescription
	{
		uint32_t Width, Height;
		bool WindowExtent = false;

		ImageFlags Flags;
		ImageFormat Format = RGBA16;
	};

	class Image
	{
	public:
		virtual ~Image() = default;

		virtual void* GetImGuiTexture() = 0;
		virtual uint32_t GetWidth() = 0;
		virtual uint32_t GetHeight() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Resize(const glm::vec2& size) = 0;

		virtual void Destroy() = 0;

		static Ref<Image> Create(const ImageDescription& desc);
	};

}