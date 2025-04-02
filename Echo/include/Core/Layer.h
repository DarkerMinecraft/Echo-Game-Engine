#pragma once

#include "Base.h"
#include "Events/Event.h"

#include <string>
#include "Timestep.h"

namespace Echo 
{

	class Layer 
	{
	public:
		Layer(const std::string& name = "Layer")
			: m_DebugName(name) {}
		virtual ~Layer() {};

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnImGuiRender() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnEvent(Event& e) {}

		virtual void Start() {}
		virtual void End() {}

		virtual void Destroy() {}

		const std::string& GetName() const { return m_DebugName; }
	private:
		std::string m_DebugName;
	};

}