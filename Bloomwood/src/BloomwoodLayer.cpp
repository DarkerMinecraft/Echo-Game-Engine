#include "BloomwoodLayer.h"

namespace Bloomwood 
{

	BloomwoodLayer::BloomwoodLayer()
	{

	}

	void BloomwoodLayer::OnAttach()
	{
		Echo::FramebufferSpecification renderSpecs{};
		renderSpecs.UseSamples = true;
		renderSpecs.WindowExtent = true;
		renderSpecs.Attachments = { Echo::FramebufferTextureFormat::RGBA8 };
		m_RenderFramebuffer = Echo::Framebuffer::Create(renderSpecs);

		Echo::FramebufferSpecification noSamplesSpecs{};
		noSamplesSpecs.WindowExtent = true;
		noSamplesSpecs.Attachments = { Echo::FramebufferTextureFormat::RGBA8 };
		m_NoSamplesFramebuffer = Echo::Framebuffer::Create(noSamplesSpecs);

		Echo::FramebufferSpecification finalSpecs{};
		finalSpecs.WindowExtent = true;
		finalSpecs.Attachments = { Echo::FramebufferTextureFormat::RGBA8 };
		m_FinalFramebuffer = Echo::Framebuffer::Create(finalSpecs);

		m_PresentShader = Echo::AssetRegistry::LoadAsset<Echo::ShaderAsset>("shaders/presentShader.slang");

		Echo::PipelineSpecification presentSpec{};
		presentSpec.CullMode = Echo::Cull::None;
		presentSpec.RenderTarget = m_FinalFramebuffer;

		m_PresentPipeline = Echo::Pipeline::Create(m_PresentShader->GetShader(), presentSpec);
		m_PresentShader->SetPipeline(m_PresentPipeline);
	}

	void BloomwoodLayer::OnDetach()
	{

	}

	void BloomwoodLayer::OnUpdate(Echo::Timestep ts)
	{
		{
			Echo::CommandList cmd;
			cmd.SetSourceFramebuffer(m_RenderFramebuffer);

			cmd.Begin();
			cmd.ClearColor(m_RenderFramebuffer, 0, { 0.3f, 0.3f, 0.3f, 0.3f });
			cmd.Execute();
			m_RenderFramebuffer->ResolveToFramebuffer(m_NoSamplesFramebuffer.get());
		}

		{
			Echo::CommandList cmd;
			cmd.SetShouldPresent(true);
			cmd.SetSourceFramebuffer(m_FinalFramebuffer);

			cmd.Begin();
			cmd.BeginRendering(m_FinalFramebuffer);
			cmd.BindPipeline(m_PresentPipeline);
			m_PresentPipeline->BindResource(0, 0, m_NoSamplesFramebuffer, 0);
			cmd.Draw(3, 1, 0, 0);
			cmd.EndRendering();
			cmd.Execute();
		}
	}

	void BloomwoodLayer::OnEvent(Echo::Event& e)
	{

	}

	void BloomwoodLayer::OnImGuiRender()
	{

	}

	void BloomwoodLayer::Destroy()
	{

	}

}