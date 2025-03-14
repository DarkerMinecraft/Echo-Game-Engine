#pragma once

#include "Echo/Core/Base.h"

#include <glm/glm.hpp>

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
		bool DrawImage = false;
		bool DrawImageExtent = false;
		bool DrawToImGui = false;

		ImageFormat Format = RGBA16;
	};

	class Image
	{
	public:
		virtual ~Image() = default;

		virtual void* GetColorAttachmentID() = 0;
		virtual uint32_t GetWidth() = 0;
		virtual uint32_t GetHeight() = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;
		virtual void Resize(const glm::vec2& size) = 0;

		virtual void Destroy() = 0;

		static Ref<Image> Create(const ImageDescription& desc);
	};

}