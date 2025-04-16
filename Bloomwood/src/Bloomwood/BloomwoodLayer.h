#pragma once

#include <Echo.h>

namespace Bloomwood 
{
	class BloomwoodLayer : public Echo::Layer
	{
	public:
		BloomwoodLayer() = default;
		virtual ~BloomwoodLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(Echo::Timestep ts) override;
		virtual void OnImGuiRender() override;
		virtual void OnEvent(Echo::Event& event) override;
	private:
		Echo::Ref<Echo::Framebuffer> m_Framebuffer;
	};

}