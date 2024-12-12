#include "pch.h"
#include "Texture.h"

#include "Echo/Core/Application.h"

#include <Platform/Vulkan/VulkanTexture.h>

namespace Echo 
{

	Ref<Texture> Texture::Create(const std::string& texturePath)
	{
		GraphicsAPI api = Application::Get().GetWindow().GetDevice()->GetGraphicsAPI();
		switch (api)
		{
			case GraphicsAPI::Vulkan: return CreateRef<VulkanTexture>(texturePath);
			case GraphicsAPI::DirectX12: return nullptr;
			default: return nullptr;
		}
		return nullptr;
	}

}