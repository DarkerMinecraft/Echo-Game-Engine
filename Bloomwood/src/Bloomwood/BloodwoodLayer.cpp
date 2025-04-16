#include "BloomwoodLayer.h"

#include <imgui.h>

namespace Bloomwood
{

	void BloomwoodLayer::OnAttach()
	{
		Echo::FramebufferSpecification fbSpec;
		fbSpec.WindowExtent = true;
		fbSpec.Attachments = { Echo::FramebufferTextureFormat::RGBA8 };

		m_Framebuffer = Echo::Framebuffer::Create(fbSpec);
	}

	void BloomwoodLayer::OnDetach()
	{

	}

	void BloomwoodLayer::OnUpdate(Echo::Timestep ts)
	{
		Echo::CommandList cmd;
		cmd.SetSourceFramebuffer(m_Framebuffer);
		cmd.SetShouldPresent(true);

		cmd.Begin();
		cmd.ClearColor(m_Framebuffer, 0, { 1.0f, 0.6f, 0.1f, 1.0f });
		cmd.Execute();
	}

	void BloomwoodLayer::OnImGuiRender()
	{

	}

	void BloomwoodLayer::OnEvent(Echo::Event& event)
	{

	}

}