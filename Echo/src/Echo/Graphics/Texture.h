#pragma once

#include "Core/Base.h"
#include "RHISpecification.h"

#include <filesystem>

namespace Echo 
{

	class Texture 
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() = 0;
		virtual uint32_t GetHeight() = 0;

		virtual void Destroy() = 0;

		virtual bool operator==(const Texture& other) const = 0;
	};

	class Texture2D : public Texture 
	{
	public:
		virtual ~Texture2D() = default;

		virtual void* GetImGuiResourceID() = 0;

		static Ref<Texture2D> Create(const std::filesystem::path& path, const Texture2DSpecification& spec);
		static Ref<Texture2D> Create(uint32_t width, uint32_t height, void* data);
	};

}