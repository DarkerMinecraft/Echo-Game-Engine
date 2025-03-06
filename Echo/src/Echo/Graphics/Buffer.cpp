#include "pch.h"
#include "Buffer.h"

#include "Echo/Core/Application.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace Echo 
{


	Ref<VertexBuffer> VertexBuffer::Create(std::vector<float> vertices)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanVertexBuffer>(device, vertices);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(std::vector<float> vertices, std::vector<float> color)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanVertexBuffer>(device, vertices, color);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(std::vector<uint32_t> indices)
	{
		Device* device = Application::Get().GetWindow().GetDevice();

		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanIndexBuffer>(device, indices);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}