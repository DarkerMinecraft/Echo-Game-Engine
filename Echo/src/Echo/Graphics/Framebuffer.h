#pragma once

#include "Echo/Core/Base.h"

#include <glm/glm.hpp>

namespace Echo
{

	enum FramebufferTextureFormat
	{
		RGBA8 = 1,
		RGBA16F = 2,
		RGBA32F = 3,
		R32F = 4,
		RG32F = 5,
		RGB32F = 6,
		RGBA16 = 7,
		RGBA8Srgb = 8,
		BGRA8 = 9,
		Depth32F = 10,
		Depth24Stencil8 = 11
	};

	struct FramebufferTextureSpecification 
	{
		FramebufferTextureSpecification() = default;
		FramebufferTextureSpecification(FramebufferTextureFormat format) : TextureFormat(format) {}

		FramebufferTextureFormat TextureFormat;
	};

	struct FramebufferAttachmentSpecification 
	{
		FramebufferAttachmentSpecification() = default;
		FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) 
			: Attachments(attachments) {}

		std::vector<FramebufferTextureSpecification> Attachments;
	};

	struct FramebufferSpecification
	{
		uint32_t Width, Height;
		bool WindowExtent = false;

		FramebufferAttachmentSpecification Attachments;
	};

	class Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

		virtual uint32_t GetWidth() = 0;
		virtual uint32_t GetHeight() = 0;
		virtual void* GetImGuiTexture(uint32_t index) = 0;

		virtual void Resize(uint32_t width, uint32_t height) = 0;

		virtual void Destroy() = 0;

		static Ref<Framebuffer> Create(const FramebufferSpecification& desc);
	};

}