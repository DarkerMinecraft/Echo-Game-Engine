#include "pch.h"
#include "Model.h"

#include "Device.h"
#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanModel.h"

namespace Echo 
{

	Ref<Model> Model::Create(std::vector<uint32_t> indices, std::vector<Vertex> vertices, Ref<Texture> texture)
	{
		GraphicsAPI api = Application::Get().GetWindow().GetDevice()->GetGraphicsAPI();
		switch (api)
		{
			case GraphicsAPI::Vulkan: return CreateRef<VulkanModel>(indices, vertices, texture);
			case GraphicsAPI::DirectX12: return nullptr;
			default: return nullptr;
		}
		return nullptr;
	}

	Ref<Model> Model::Create(const std::string& objFilePath, Ref<Texture> texture)
	{
		GraphicsAPI api = Application::Get().GetWindow().GetDevice()->GetGraphicsAPI();
		switch (api)
		{
			case GraphicsAPI::Vulkan: return CreateRef<VulkanModel>(objFilePath, texture);
			case GraphicsAPI::DirectX12: return nullptr;
			default: return nullptr;
		}
		return nullptr;
	}

}