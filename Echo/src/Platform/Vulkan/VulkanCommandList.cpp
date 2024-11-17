#include "pch.h"
#include "VulkanCommandList.h"

#include "VulkanResource.h"
#include "VulkanSwapchain.h"

#include "Echo/Core/Application.h"

namespace Echo 
{

	VulkanCommandBuffer::VulkanCommandBuffer(VulkanDevice* device, VkCommandPool commandPool)
		: m_Device(device), m_CommandPool(commandPool)
	{
		CreateCommandBuffer();
	}

	VulkanCommandBuffer::~VulkanCommandBuffer()
	{

	}

	void VulkanCommandBuffer::AddMesh(Ref<Resource> resource, Ref<Model> model)
	{
		if (resource->GetAssetResource() == AssetResource::GraphicsShader) 
		{
			if (m_Meshes.contains(resource)) 
			{
				std::vector<VulkanModel*> batch = m_Meshes[resource];
				batch.emplace_back((VulkanModel*) model.get());
			}
			else 
			{
				std::vector<VulkanModel*> batch;
				batch.emplace_back((VulkanModel*) model.get());

				m_Meshes[resource] = batch;
			}
		}
	}

	void VulkanCommandBuffer::Begin()
	{
		m_ImageCount = m_Device->GetSwapchain()->AcquireNextImage();

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}

		m_Device->GetSwapchain()->StartRenderPass(m_ImageCount);
		m_Device->GetSwapchain()->SetState();
	}

	void VulkanCommandBuffer::Submit()
	{
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { m_Device->GetFrame().ImageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_CommandBuffer;

		VkSemaphore signalSemaphores[] = { m_Device->GetFrame().RenderFinishedSemaphore };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		for (auto& [resource, models] : m_Meshes)
		{
			resource->Bind();
			for (VulkanModel* model : models) 
			{
				model->Bind();
				
				vkCmdDraw(m_CommandBuffer, model->GetVertexCount(), 1, 0, 0);
			}
		}

		if (vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_Device->GetFrame().InFlightFence) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { ((VulkanSwapchain*) m_Device->GetSwapchain())->GetSwapchain() };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &m_ImageCount;
		presentInfo.pResults = nullptr; // Optional

		vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);
	}

	void VulkanCommandBuffer::End()
	{
		m_Device->GetSwapchain()->EndRenderPass();

		if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}
	}

	void VulkanCommandBuffer::CreateCommandBuffer()
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_Device->GetDevice(), &allocInfo, &m_CommandBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate command buffers!");
		}
	}

	VulkanVertexBuffer::VulkanVertexBuffer(std::vector<Vertex> vertices)
		: m_Device((VulkanDevice*) Application::Get().GetWindow().GetDevice())
	{
		CreateVertexBuffer(vertices);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		vkDestroyBuffer(m_Device->GetDevice(), m_VertexBuffer, nullptr);
		vkFreeMemory(m_Device->GetDevice(), m_VertexBufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Bind()
	{
		VkBuffer vertexBuffers[] = { m_VertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(((VulkanCommandBuffer*) m_Device->GetCommandBuffer())->GetBuffer(), 0, 1, vertexBuffers, offsets);
	}

	void VulkanVertexBuffer::CreateVertexBuffer(std::vector<Vertex> vertices)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = sizeof(vertices[0]) * vertices.size();

		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_Device->GetDevice(), &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create vertex buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_Device->GetDevice(), m_VertexBuffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = m_Device->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		if (vkAllocateMemory(m_Device->GetDevice(), &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to allocate vertex buffer memory!");
		}

		vkBindBufferMemory(m_Device->GetDevice(), m_VertexBuffer, m_VertexBufferMemory, 0);

		void* data;
		vkMapMemory(m_Device->GetDevice(), m_VertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, vertices.data(), (size_t)bufferInfo.size);
		vkUnmapMemory(m_Device->GetDevice(), m_VertexBufferMemory);
	}

}