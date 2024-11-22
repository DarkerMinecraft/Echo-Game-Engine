#include "pch.h"
#include "Model.h"

#include "Device.h"
#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanModel.h"

namespace Echo 
{

	Ref<Model> Model::Create(std::vector<uint32_t> indices, std::vector<Vertex> vertices)
	{
		GraphicsAPI api = Application::Get().GetWindow().GetDevice()->GetGraphicsAPI();
		switch (api)
		{
			case GraphicsAPI::Vulkan: return CreateRef<VulkanModel>(indices, vertices);
			case GraphicsAPI::DirectX12: return nullptr;
			default: return nullptr;
		}
		return nullptr;
	}

}