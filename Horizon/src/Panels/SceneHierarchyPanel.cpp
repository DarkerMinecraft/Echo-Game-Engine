#include "SceneHierarchyPanel.h"
#include "Echo/Scene/Components.h"

#include <glm/gtc/type_ptr.hpp>

#include <imgui.h>
#include <imgui_internal.h>

namespace Echo 
{

	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
	{
		SetContext(scene);
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
	{
		m_Context = scene;
		m_SelectionContext = {};

		m_EntityComponentPanel.SetContext(scene);
	}

	void SceneHierarchyPanel::SetSelectedEntity(int entityID)
	{
		m_SelectionContext = Entity{ (entt::entity)(uint32_t)entityID, m_Context.get() };
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		ImGui::Begin("Scene Hierarchy");
		m_Context->m_Registry.view<entt::entity>().each([&](auto entityID) 
		{
			Entity entity{ entityID, m_Context.get() };
			DrawEntityNode(entity);
		});

		if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight)) 
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				m_Context->CreateEntity();
			}
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Properties");
		if (m_SelectionContext)
		{
			m_EntityComponentPanel.OnImGuiRenderEntityNode(m_SelectionContext);
		}
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0)  | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());
		if (ImGui::IsItemClicked()) 
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Destroy Entity"))
			{
				entityDeleted = true;
			}
			ImGui::EndPopup();
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = {};
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			if (m_SelectionContext == entity)
			{
				m_SelectionContext = {};
			}
			m_Context->DestroyEntity(entity);
		}
	}
	
}