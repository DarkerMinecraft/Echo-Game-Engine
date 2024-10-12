#pragma once

#include "Echo/Graphics/RendererAPI.h"

namespace Echo
{
	class DirectX12RendererAPI : public RendererAPI
	{
		virtual void Init() override;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;
	};
}
