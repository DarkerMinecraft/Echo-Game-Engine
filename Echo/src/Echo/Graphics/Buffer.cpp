#include "pch.h"
#include "Buffer.h"

#include "Core/Application.h"
#include "Vulkan/VulkanBuffer.h"
#include "Utils/DeferredInitManager.h"

namespace Echo 
{
	Ref<VertexBuffer> VertexBuffer::Create(float* data, uint32_t size, bool isDynamic)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[VertexBuffer] Device not ready, deferring vertex buffer creation (with data)");
			Echo::DeferredInitManager::Enqueue([dataCopy = std::vector<float>(data, data + size / sizeof(float)), size, isDynamic]() mutable {
				EC_CORE_INFO("[VertexBuffer] Running deferred vertex buffer creation (device now ready)");
				Device* device = Application::Get().GetWindow().GetDevice();
				if (device && device->IsInitialized()) {
					auto buffer = CreateScope<VulkanVertexBuffer>(device, dataCopy.data(), size, isDynamic);
				} else {
					EC_CORE_ERROR("[VertexBuffer] Device still not ready in deferred task!");
				}
			});
			return nullptr;
		}
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanVertexBuffer>(device, data, size, isDynamic);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size, bool isDynamic)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[VertexBuffer] Device not ready, deferring vertex buffer creation (no data)");
			Echo::DeferredInitManager::Enqueue([size, isDynamic]() {
				EC_CORE_INFO("[VertexBuffer] Running deferred vertex buffer creation (device now ready)");
				Device* device = Application::Get().GetWindow().GetDevice();
				if (device && device->IsInitialized()) {
					auto buffer = CreateScope<VulkanVertexBuffer>(device, size, isDynamic);
				} else {
					EC_CORE_ERROR("[VertexBuffer] Device still not ready in deferred task!");
				}
			});
			return nullptr;
		}
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanVertexBuffer>(device, size, isDynamic);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(std::vector<uint32_t> indices)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[IndexBuffer] Device not ready, deferring index buffer creation (vector)");
			Echo::DeferredInitManager::Enqueue([indicesCopy = indices]() mutable {
				EC_CORE_INFO("[IndexBuffer] Running deferred index buffer creation (device now ready)");
				Device* device = Application::Get().GetWindow().GetDevice();
				if (device && device->IsInitialized()) {
					auto buffer = CreateScope<VulkanIndexBuffer>(device, indicesCopy);
				} else {
					EC_CORE_ERROR("[IndexBuffer] Device still not ready in deferred task!");
				}
			});
			return nullptr;
		}
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanIndexBuffer>(device, indices);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(uint32_t* indices, uint32_t count)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[IndexBuffer] Device not ready, deferring index buffer creation (raw)");
			Echo::DeferredInitManager::Enqueue([indicesCopy = std::vector<uint32_t>(indices, indices + count), count]() mutable {
				EC_CORE_INFO("[IndexBuffer] Running deferred index buffer creation (device now ready)");
				Device* device = Application::Get().GetWindow().GetDevice();
				if (device && device->IsInitialized()) {
					auto buffer = CreateScope<VulkanIndexBuffer>(device, indicesCopy.data(), count);
				} else {
					EC_CORE_ERROR("[IndexBuffer] Device still not ready in deferred task!");
				}
			});
			return nullptr;
		}
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanIndexBuffer>(device, indices, count);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndirectBuffer> IndirectBuffer::Create()
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[IndirectBuffer] Device not ready, deferring indirect buffer creation");
			Echo::DeferredInitManager::Enqueue([]() {
				EC_CORE_INFO("[IndirectBuffer] Running deferred indirect buffer creation (device now ready)");
				Device* device = Application::Get().GetWindow().GetDevice();
				if (device && device->IsInitialized()) {
					auto buffer = CreateScope<VulkanIndirectBuffer>(device);
				} else {
					EC_CORE_ERROR("[IndirectBuffer] Device still not ready in deferred task!");
				}
			});
			return nullptr;
		}
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanIndirectBuffer>(device);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<UniformBuffer> UniformBuffer::Create(void* data, uint32_t size)
	{
		Device* device = Application::Get().GetWindow().GetDevice();
		if (!device || !device->IsInitialized()) {
			EC_CORE_WARN("[UniformBuffer] Device not ready, deferring uniform buffer creation");
			Echo::DeferredInitManager::Enqueue([dataCopy = std::vector<uint8_t>((uint8_t*)data, (uint8_t*)data + size), size]() mutable {
				EC_CORE_INFO("[UniformBuffer] Running deferred uniform buffer creation (device now ready)");
				Device* device = Application::Get().GetWindow().GetDevice();
				if (device && device->IsInitialized()) {
					auto buffer = CreateScope<VulkanUniformBuffer>(device, dataCopy.data(), size);
					// Note: The returned buffer is not captured here; user must re-request or handle via callback if needed.
				} else {
					EC_CORE_ERROR("[UniformBuffer] Device still not ready in deferred task!");
				}
			});
			return nullptr;
		}
		switch (device->GetDeviceType())
		{
			case DeviceType::Vulkan:  return CreateScope<VulkanUniformBuffer>(device, data, size);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}