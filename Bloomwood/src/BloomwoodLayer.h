#pragma once

#include <Echo.h>

namespace Bloomwood 
{

	class BloomwoodLayer : public Echo::Layer 
	{
	public:
		BloomwoodLayer();
		virtual ~BloomwoodLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Echo::Timestep ts) override;

		virtual void OnEvent(Echo::Event& e) override;

		virtual void OnImGuiRender() override;

		virtual void Destroy() override;
	private:
		Ref<Echo::Framebuffer> m_RenderFramebuffer;
		Ref<Echo::Framebuffer> m_NoSamplesFramebuffer;
		Ref<Echo::Framebuffer> m_FinalFramebuffer;

		Ref<Echo::Pipeline> m_PresentPipeline;
		Ref<Echo::ShaderAsset> m_PresentShader;
	};

}