#include "pch.h"
#include "GraphicsModel.h"

#include "Renderer.h"
#include "Platform/Vulkan/VulkanModel.h"

namespace Echo 
{

	Ref<GraphicsModel> GraphicsModel::Create(const std::vector<Vertex>& vertices)
	{
		switch (RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::Vulkan: return CreateRef<VulkanModel>(vertices);
			case RendererAPI::API::DirectX12: return nullptr;
		}
	}

}