#include "pch.h"

#include "Model.h"
#include "Device.h"
#include "Echo/Core/Application.h"

#include "Platform/Vulkan/VulkanCommandList.h"

namespace Echo 
{

	Ref<Model> Model::CreateModel(const std::vector<Vertex> vertices)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		switch (device->GetGraphicsAPI())
		{
			case API::DirectX: return nullptr;
			case API::Vulkan: return CreateScope<VulkanModel>(vertices);
		}
	}

}