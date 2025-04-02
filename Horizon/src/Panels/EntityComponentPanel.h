#pragma once

#include "Scene/Scene.h"

namespace Echo 
{

	class EntityComponentPanel 
	{
	public:
		EntityComponentPanel() = default;
		EntityComponentPanel(const Ref<Scene>& context)
			: m_Context(context)
		{};
		void SetContext(const Ref<Scene>& context) { m_Context = context; };

		void OnImGuiRenderEntityNode(Entity entity);
	private:
		Ref<Scene> m_Context = nullptr;
	};

}