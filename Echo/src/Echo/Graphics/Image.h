#pragma once

#include "Echo/Core/Base.h"

namespace Echo
{

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
		bool DrawImage;
		bool DrawImageExtent = false;

		ImageFormat Format = RGBA16;
	};

	class Image
	{
	public:
		virtual ~Image() = default;

		virtual uint32_t GetWidth() = 0;
		virtual uint32_t GetHeight() = 0;

		virtual void Destroy() = 0;

		virtual void* GetImageHandle() = 0;

		static Ref<Image> Create(const ImageDescription& desc);
	};

}