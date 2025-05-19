#include "pch.h"
#include "VulkanFramebuffer.h"
#include "Vulkan/Utils/VulkanInitializers.h"

#include "VulkanCommandBuffer.h"

#include "backends/imgui_impl_vulkan.h"
#include "Utils/VulkanDescriptors.h"
#include "Utils/VulkanImages.h"

namespace Echo
{

	VulkanFramebuffer::VulkanFramebuffer(Device* device, const FramebufferSpecification& specification)
		: m_Device((VulkanDevice*)device)
	{
		CreateAllocatedFramebuffers(specification);
		m_Device->AddFramebuffer(this);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		Destroy();
	}

	uint32_t VulkanFramebuffer::GetWidth()
	{
		return m_Width;
	}

	uint32_t VulkanFramebuffer::GetHeight()
	{
		return m_Height;
	}

	void* VulkanFramebuffer::GetImGuiTexture(uint32_t index)
	{
		if (GetCurrentLayout(index) != VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				TransitionImageLayout(cmd, index, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			});
		}

		if (m_DescriptorSet)
		{
			ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);
		}

		m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Framebuffers[index].Sampler, m_Framebuffers[index].ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

		std::vector<VulkanFramebuffer*> framebuffers = m_Device->GetImGuiFramebuffers();
		if (std::find(framebuffers.begin(), framebuffers.end(), this) == framebuffers.end())
		{
			m_Device->AddImGuiFramebuffer(this);
		}

		return m_DescriptorSet;
	}

	void VulkanFramebuffer::Resize(uint32_t width, uint32_t height)
	{
		m_Width = width;
		m_Height = height;

		for (int i = 0; i < m_Framebuffers.size(); i++)
		{
			if (m_Framebuffers[i].ImageExtent.width == width && m_Framebuffers[i].ImageExtent.height == height)
				return;

			CreateImage(i, width, height);

			m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
			{
				VulkanImages::TransitionImage(cmd, m_Framebuffers[i].Image, VK_IMAGE_LAYOUT_UNDEFINED, GetCurrentLayout(i));
			});
		}
	}

	int VulkanFramebuffer::ReadPixel(uint32_t index, uint32_t x, uint32_t y)
	{
		AllocatedBuffer stagingBuffer = m_Device->CreateBuffer(sizeof(int32_t), VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			if (GetCurrentLayout(index) != VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
			{
				TransitionImageLayout(cmd, index, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			}

			VulkanImages::CopyImageToBuffer(cmd, stagingBuffer.Buffer, m_Framebuffers[index].Image, { x, y });
		});

		int32_t pixel;
		void* data;
		data = m_Device->GetMappedData(stagingBuffer);
		memcpy(&pixel, data, sizeof(int32_t));
		m_Device->DestroyBuffer(stagingBuffer);

		return pixel;
	}

	void VulkanFramebuffer::ResolveToFramebuffer(CommandBuffer* cmd, Framebuffer* targetFramebuffer)
	{
		if (!m_UseSamples) return;

		VkCommandBuffer commandBuffer = ((VulkanCommandBuffer*)cmd)->GetCommandBuffer();
		VulkanFramebuffer* framebuffer = (VulkanFramebuffer*)targetFramebuffer;
		for (uint32_t i = 0; i < m_ColorFormats.size(); i++)
		{
			TransitionImageLayout(commandBuffer, i, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			framebuffer->TransitionImageLayout(commandBuffer, i, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			VkImageResolve resolveRegion{};
			resolveRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			resolveRegion.srcSubresource.mipLevel = 0;
			resolveRegion.srcSubresource.baseArrayLayer = 0;
			resolveRegion.srcSubresource.layerCount = 1;

			resolveRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			resolveRegion.dstSubresource.mipLevel = 0;
			resolveRegion.dstSubresource.baseArrayLayer = 0;
			resolveRegion.dstSubresource.layerCount = 1;

			resolveRegion.srcOffset = { 0, 0, 0 };
			resolveRegion.dstOffset = { 0, 0, 0 };
			resolveRegion.extent = { m_Width, m_Height, 1 };

			vkCmdResolveImage(
				commandBuffer,
				m_Framebuffers[i].Image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				framebuffer->GetImage(i).Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &resolveRegion
			);
		}
	}

	void VulkanFramebuffer::TransitionImageLayout(VkCommandBuffer cmd, uint32_t index, VkImageLayout newLayout)
	{
		VulkanImages::TransitionImage(cmd, m_Framebuffers[index].Image, m_Framebuffers[index].ImageLayout, newLayout);
		m_Framebuffers[index].ImageLayout = newLayout;
	}

	void VulkanFramebuffer::Destroy()
	{
		for (auto& framebuffer : m_Framebuffers)
		{
			if (framebuffer.Destroyed)
				continue;

			if (m_DescriptorSet)
			{
				ImGui_ImplVulkan_RemoveTexture(m_DescriptorSet);
				m_DescriptorSet = nullptr;
			}

			vkDestroySampler(m_Device->GetDevice(), framebuffer.Sampler, nullptr);
			m_Device->DestroyImage(framebuffer);

			framebuffer.Destroyed = true;
		}
	}

	void VulkanFramebuffer::UpdateSize()
	{
		for (int i = 0; i < m_Framebuffers.size(); i++)
		{
			if (m_WindowExtent)
			{
				CreateImage(i, m_Device->GetDrawExtent().width, m_Device->GetDrawExtent().height);
			}
		}
	}

	void VulkanFramebuffer::CreateAllocatedFramebuffers(const FramebufferSpecification& spec)
	{
		m_WindowExtent = spec.WindowExtent;
		m_Width = spec.Width;
		m_Height = spec.Height;
		m_UseSamples = spec.UseSamples;

		for (int i = 0; i < spec.Attachments.Attachments.size(); i++)
		{
			m_Attachments.push_back(spec.Attachments.Attachments[i].TextureFormat);
		}

		if (spec.WindowExtent)
		{
			m_Width = m_Device->GetDrawExtent().width;
			m_Height = m_Device->GetDrawExtent().height;
		}

		VkExtent3D drawImageExtent = {
			m_Width,
			m_Height,
			1
		};

		auto MapFramebufferFormat = [](FramebufferTextureFormat format) -> VkFormat
		{
			switch (format)
			{
				case RGBA8:
					return VK_FORMAT_R8G8B8A8_UNORM;
				case RedInt:
					return VK_FORMAT_R32_SINT;
				case Depth32F:
					return VK_FORMAT_D32_SFLOAT;
				case BGRA8:
					return VK_FORMAT_B8G8R8A8_UNORM;
				case Depth24Stencil8:
					return VK_FORMAT_D24_UNORM_S8_UINT;
				default:
					return VK_FORMAT_UNDEFINED;
			}
		};

		for (int i = 0; i < spec.Attachments.Attachments.size(); i++)
		{
			FramebufferTextureSpecification attachment = spec.Attachments.Attachments[i];
			VkImageUsageFlags attachmentUsage;
			if (attachment.TextureFormat >= 10)
				attachmentUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
			else
				attachmentUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;

			AllocatedImage image;
			if (spec.UseSamples)
			{
				image = m_Device->CreateImage(drawImageExtent, MapFramebufferFormat(attachment.TextureFormat), VK_IMAGE_USAGE_TRANSFER_DST_BIT
											  | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | attachmentUsage);
			}
			else
			{
				image = m_Device->CreateImageNoMSAA(drawImageExtent, MapFramebufferFormat(attachment.TextureFormat), VK_IMAGE_USAGE_TRANSFER_DST_BIT
													| VK_IMAGE_USAGE_TRANSFER_SRC_BIT | attachmentUsage);
			}

			if (attachment.TextureFormat >= 10)
			{
				image.DepthTexture = true;
				m_DepthIndex = i;
			}
			else
			{
				m_ColorFormats.push_back(MapFramebufferFormat(attachment.TextureFormat));
			}

			VkSamplerCreateInfo sampl = { .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO };

			VkSampler sampler;
			sampl.magFilter = VK_FILTER_NEAREST;
			sampl.minFilter = VK_FILTER_NEAREST;

			vkCreateSampler(m_Device->GetDevice(), &sampl, nullptr, &sampler);
			image.Sampler = sampler;

			image.ImageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			m_Framebuffers.push_back(image);
		}
	}

	void VulkanFramebuffer::CreateImage(uint32_t index, uint32_t width, uint32_t height)
	{
		AllocatedImage oldImage = m_Framebuffers[index];
		VkImageUsageFlags attachmentUsage;
		if (m_Attachments[index] >= 10)
			attachmentUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		else
			attachmentUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
		AllocatedImage image;
			
		if (m_UseSamples)
		{
			image = m_Device->CreateImage({ width, height, 1 }, oldImage.ImageFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT
										  | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | attachmentUsage);
		}
		else 
		{
			image = m_Device->CreateImageNoMSAA({ width, height, 1 }, oldImage.ImageFormat, VK_IMAGE_USAGE_TRANSFER_DST_BIT
										  | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | attachmentUsage);
		}

		m_Width = width;
		m_Height = height;

		image.ImageLayout = oldImage.ImageLayout;
		image.DepthTexture = oldImage.DepthTexture;
		image.Sampler = oldImage.Sampler;

		m_Framebuffers[index] = image;

		m_Device->DestroyImage(oldImage);
		m_Device->ImmediateSubmit([&](VkCommandBuffer cmd)
		{
			VulkanImages::TransitionImage(cmd, m_Framebuffers[index].Image, VK_IMAGE_LAYOUT_UNDEFINED, m_Framebuffers[index].ImageLayout);
		});
	}

}