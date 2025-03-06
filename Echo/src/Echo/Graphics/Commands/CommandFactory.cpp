#include "pch.h"
#include "CommandFactory.h"

#include "Platform/Vulkan/Commands/VulkanTransitionImageCommand.h"
#include "Platform/Vulkan/Commands/VulkanClearColorCommand.h"
#include "Platform/Vulkan/Commands/VulkanBindPipelineCommand.h"
#include "Platform/Vulkan/Commands/VulkanDispatchCommand.h"
#include "Platform/Vulkan/Commands/VulkanBindBufferCommand.h"
#include "Platform/Vulkan/Commands/VulkanRenderingCommand.h"
#include "Platform/Vulkan/Commands/VulkanDrawCommand.h"
#include "Platform/Vulkan/Commands/VulkanCopyImageToImageCommand.h"
#include "Platform/Vulkan/Commands/VulkanRenderImGuiCommand.h"

namespace Echo 
{

	Ref<ICommand> CommandFactory::TransitionImageCommand(Ref<Image> image, ImageLayout oldLayout, ImageLayout newLayout)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanTransitionImageCommand>(image, oldLayout, newLayout);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::CopyImageToImageCommand(Ref<Image> srcImage, Ref<Image> dstImage)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanCopyImageToImageCommand>(srcImage, dstImage);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::ClearColorCommand(Ref<Image> image, const glm::vec4& clearValues)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanClearColorCommand>(image, clearValues);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::BindPipelineCommand(Ref<Pipeline> pipeline)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanBindPipelineCommand>(pipeline);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::DispatchCommand(float x, float y, float z)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanDispatchCommand>(x, y, z);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::BindVertexBufferCommand(Ref<VertexBuffer> vertexBuffer)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanBindVertexBufferCommand>(vertexBuffer);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::BindIndicesBufferCommand(Ref<IndexBuffer> indexBuffer)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanBindIndicesBufferCommand>(indexBuffer);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::DrawCommand(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanDrawCommand>(vertexCount, instanceCount, firstVertex, firstInstance);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::DrawIndexedCommand(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t vertexOffset, uint32_t firstInstance)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanDrawIndexedCommand>(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
		}
	}

	Ref<ICommand> CommandFactory::BeginRenderingCommand(Ref<Image> image)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanBeginRenderingCommand>(image);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::EndRenderingCommand()
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanEndRenderingCommand>();
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::RenderImGuiCommand()
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanRenderImGuiCommand>();
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}