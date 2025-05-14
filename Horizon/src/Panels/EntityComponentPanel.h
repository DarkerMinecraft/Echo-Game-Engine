#pragma once

#include "Scene/Scene.h"

#include <filesystem>

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

		void SetCurrentDirectory(const std::filesystem::path currentDirectory) { m_CurrentDirectory = currentDirectory; }
	private:
		Ref<Scene> m_Context = nullptr;

		std::filesystem::path m_CurrentDirectory;
	};

}