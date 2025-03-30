#include "EntityComponentPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Echo/Scene/Components.h"
#include "Echo/Scene/Entity.h"

#include <filesystem>

namespace Echo
{

	extern const std::filesystem::path g_AssetPath;

	static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValues = 0.0f, float colWidth = 100)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[0];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, colWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValues;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValues;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValues;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1f, 0, 0, "%.2f");
		ImGui::PopItemWidth();

		ImGui::PopStyleVar();
		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	static void DrawComponent(const std::string& name, Entity entity, UIFunction uiFunction)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_FramePadding;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();
			ImGui::SameLine(contentRegionAvailable.x - lineHeight * 0.5f);
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}
			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
				{
					removeComponent = true;
				}
				ImGui::EndPopup();
			}
			if (open)
			{
				uiFunction(component);
				ImGui::TreePop();
			}
			if (removeComponent)
			{
				entity.RemoveComponent<T>();
			}
		}
	}

	void EntityComponentPanel::OnImGuiRenderEntityNode(Entity entity)
	{
		if (entity.GetHandle() == entt::null) return;

		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if (ImGui::InputText("##type", buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);
			}
		}

		ImGui::SameLine();
		ImGui::PushItemWidth(-1);

		if (ImGui::Button("Add Component"))
		{
			ImGui::OpenPopup("AddComponent");
		}

		if (ImGui::BeginPopup("AddComponent"))
		{
			if (ImGui::MenuItem("Sprite Renderer"))
			{
				if (!entity.HasComponent<SpriteRendererComponent>())
				{
					entity.AddComponent<SpriteRendererComponent>();
				}
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Camera"))
			{
				if (!entity.HasComponent<CameraComponent>())
				{
					entity.AddComponent<CameraComponent>();
				}
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
		{
			DrawVec3Control("Translation", component.Translation);
			glm::vec3 rotation = glm::degrees(component.Rotation);
			DrawVec3Control("Rotation", rotation);
			component.Rotation = glm::radians(rotation);
			DrawVec3Control("Scale", component.Scale, 1.0f);
		});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::Button("Texture", ImVec2(100.0f, 0));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = std::filesystem::path(g_AssetPath) / path;

					component.Texture = Texture2D::Create(texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});

		DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
		{
			ImGui::Checkbox("Primary", &component.Primary);

			const char* projectionTypeStrings[] = { "Perspective", "Orthographic" };
			const char* currentProjectionTypeString = projectionTypeStrings[(int)component.Camera.GetProjectionType()];

			if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
			{
				for (int i = 0; i < 2; i++)
				{
					bool isSelected = currentProjectionTypeString == projectionTypeStrings[i];
					if (ImGui::Selectable(projectionTypeStrings[i], isSelected))
					{
						currentProjectionTypeString = projectionTypeStrings[i];
						component.Camera.SetProjectionType((SceneCamera::ProjectionType)i);
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			if (component.Camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
			{
				float perspectiveFOV = glm::degrees(component.Camera.GetPerspectiveFOV());
				if (ImGui::DragFloat("FOV", &perspectiveFOV))
				{
					component.Camera.SetPerspectiveFOV(glm::radians(perspectiveFOV));
				}
				float perspectiveNear = component.Camera.GetPerspectiveNearClip();
				if (ImGui::DragFloat("Near Clip", &perspectiveNear))
				{
					component.Camera.SetPerspectiveNearClip(perspectiveNear);
				}
				float perspectiveFar = component.Camera.GetPerspectiveFarClip();
				if (ImGui::DragFloat("Far Clip", &perspectiveFar))
				{
					component.Camera.SetPerspectiveFarClip(perspectiveFar);
				}
			}
			else
			{
				float orthographicSize = component.Camera.GetOrthographicSize();
				if (ImGui::DragFloat("Size", &orthographicSize))
				{
					component.Camera.SetOrthographicSize(orthographicSize);
				}
				float orthographicNear = component.Camera.GetOrthographicNearClip();
				if (ImGui::DragFloat("Near Clip", &orthographicNear))
				{
					component.Camera.SetOrthographicNearClip(orthographicNear);
				}
				float orthographicFar = component.Camera.GetOrthographicFarClip();
				if (ImGui::DragFloat("Far Clip", &orthographicFar))
				{
					component.Camera.SetOrthographicFarClip(orthographicFar);
				}
			}

			ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
		});
	}

}