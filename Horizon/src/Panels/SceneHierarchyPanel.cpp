#include "SceneHierarchyPanel.h"
#include "Scene/Components.h"

#include <Debug/Instrumentor.h>

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
		m_SelectionContext = { entt::null, m_Context.get() };

		m_EntityComponentPanel.SetContext(scene);
	}

	void SceneHierarchyPanel::SetSelectedEntity(int entityID)
	{
		m_SelectionContext = { (entt::entity)(uint32_t)entityID, m_Context.get() };
	}

	void SceneHierarchyPanel::OnImGuiRender()
	{
		EC_PROFILE_FUNCTION();
		ImGui::Begin("Scene Hierarchy");

		if (m_Context)
		{

			const auto& displayOrder = m_Context->GetEntityDisplayOrder();
			for (const UUID& entityUUID : displayOrder)
			{
				Entity entity = m_Context->GetEntityByUUID(entityUUID);
				if (entity)
				{ 
					DrawEntityNode(entity);
				}
			}

			if (ImGui::BeginPopupContextWindow(0, ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_MouseButtonRight))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					m_Context->CreateEntity();
				}
				ImGui::EndPopup();
			}

			ImGui::InvisibleButton("drop_zone_end", ImVec2(-1, 20));
			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_REORDER"))
				{
					UUID draggedUUID = *(UUID*)payload->Data;
					m_Context->ReorderEntity(draggedUUID, displayOrder.size());
				}
				ImGui::EndDragDropTarget();
			}
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
		EC_PROFILE_FUNCTION();
		auto& tag = entity.GetComponent<TagComponent>().Tag;

		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0)  | ImGuiTreeNodeFlags_OpenOnArrow;
		flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags, tag.c_str());

		if (ImGui::BeginDragDropSource())
		{
			UUID entityUUID = entity.GetUUID();
			ImGui::SetDragDropPayload("ENTITY_REORDER", &entityUUID, sizeof(UUID));
			ImGui::Text("Moving: %s", tag.c_str());
			ImGui::EndDragDropSource();
		}

		// === DROP TARGET ===
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_REORDER"))
			{
				UUID draggedUUID = *(UUID*)payload->Data;
				UUID targetUUID = entity.GetUUID();

				// Don't reorder if dropping on self
				if (draggedUUID != targetUUID)
				{
					// Find target index in display order
					const auto& order = m_Context->GetEntityDisplayOrder();
					auto targetIt = std::find(order.begin(), order.end(), targetUUID);
					if (targetIt != order.end())
					{
						size_t targetIndex = std::distance(order.begin(), targetIt);
						m_Context->ReorderEntity(draggedUUID, targetIndex);
					}
				}
			}
			ImGui::EndDragDropTarget();
		}

		if (ImGui::IsItemClicked()) 
		{
			m_SelectionContext = entity;
		}

		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Duplicate Entity"))
			{
				m_Context->DuplicateEntity(m_SelectionContext);
			}
			if (ImGui::MenuItem("Destroy Entity"))
			{
				entityDeleted = true;
			}
			ImGui::EndPopup();
		}

		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			m_SelectionContext = { entt::null, m_Context.get() };
		}

		if (opened)
		{
			ImGui::TreePop();
		}

		if (entityDeleted)
		{
			if (m_SelectionContext == entity)
			{
				m_SelectionContext = {entt::null, m_Context.get()};
			}
			m_Context->DestroyEntity(entity);
		}
	}
	
}