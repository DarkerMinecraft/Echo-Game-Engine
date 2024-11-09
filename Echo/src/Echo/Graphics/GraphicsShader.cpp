#include "pch.h"
#include "GraphicsShader.h"

#include "Renderer.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Echo 
{
	
	Ref<GraphicsShader> GraphicsShader::Create(const std::string& vertexShader, const std::string& fragmentShader)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::Vulkan: return CreateRef<VulkanShader>(vertexShader, fragmentShader);
			case RendererAPI::API::DirectX12: return nullptr;
		}
	}

}