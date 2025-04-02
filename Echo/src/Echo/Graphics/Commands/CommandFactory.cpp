#include "pch.h"
#include "CommandFactory.h"

#include "Vulkan/Commands/VulkanBindPipelineCommand.h"
#include "Vulkan/Commands/VulkanDispatchCommand.h"
#include "Vulkan/Commands/VulkanBindBufferCommand.h"
#include "Vulkan/Commands/VulkanRenderingCommand.h"
#include "Vulkan/Commands/VulkanDrawCommand.h"
#include "Vulkan/Commands/VulkanRenderImGuiCommand.h"
#include "Vulkan/Commands/VulkanClearColorCommand.h"

namespace Echo 
{

	Ref<ICommand> CommandFactory::ClearColorCommand(Ref<Framebuffer> framebuffer, uint32_t index, const glm::vec4& clearValues)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanClearColorCommand>(framebuffer, index, clearValues);
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

	Ref<ICommand> CommandFactory::DrawIndirectIndexed(Ref<IndirectBuffer> indirectBuffer, uint32_t offset, uint32_t drawCount, uint32_t stride)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanDrawIndexedIndirect>(indirectBuffer, offset, drawCount, stride);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::BeginRenderingCommand(Ref<Framebuffer> framebuffer)
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanBeginRenderingCommand>(framebuffer);
		}
		EC_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<ICommand> CommandFactory::BeginRenderingCommand()
	{
		switch (GetDeviceType())
		{
			case DeviceType::Vulkan: return CreateRef<VulkanBeginRenderingCommand>();
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