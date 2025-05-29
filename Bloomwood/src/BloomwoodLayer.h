#pragma once

#include <Core/Layer.h>
#include <Core/Timestep.h>
#include <Events/Event.h>

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
	};

}