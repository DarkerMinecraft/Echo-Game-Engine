#include "EntityComponentPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>
#include <string>

#include "Scene/Components.h"
#include "Scene/Entity.h"

#include <filesystem>

namespace Echo
{

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
			if (!entity.HasComponent<SpriteRendererComponent>())
			{
				if (ImGui::MenuItem("Sprite Renderer"))
				{
					entity.AddComponent<SpriteRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!entity.HasComponent<CircleRendererComponent>())
			{
				if (ImGui::MenuItem("Circle Renderer"))
				{
					entity.AddComponent<CircleRendererComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!entity.HasComponent<CameraComponent>())
			{
				if (ImGui::MenuItem("Camera"))
				{
					entity.AddComponent<CameraComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!entity.HasComponent<Rigidbody2DComponent>())
			{
				if (ImGui::MenuItem("Rigidbody 2D"))
				{
					entity.AddComponent<Rigidbody2DComponent>();
					ImGui::CloseCurrentPopup();
				}
			}
			if (!entity.HasComponent<BoxCollider2DComponent>())
			{
				if (ImGui::MenuItem("Box Collider 2D"))
				{
					entity.AddComponent<BoxCollider2DComponent>();
					ImGui::CloseCurrentPopup();
				}
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

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [this](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::Button("Texture", ImVec2(100.0f, 0));

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
				{
					const wchar_t* path = (const wchar_t*)payload->Data;
					std::filesystem::path texturePath = m_CurrentDirectory / path;

					component.Texture = Texture2D::Create(texturePath.string());
				}
				ImGui::EndDragDropTarget();
			}

			ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
		});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [this](auto& component)
		{
			ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
			ImGui::DragFloat("Outline Thickness", &component.OutlineThickness, 0.1f, 0.0f, 1.0f);
			ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 0.5f);
		});

		DrawComponent<Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& component)
		{
			const char* bodyTypeStrings[] = { "Static", "Dynamic", "Kinematic" };
			const char* currentBodyTypeString = bodyTypeStrings[(int)component.Type];

			if (ImGui::BeginCombo("Type", currentBodyTypeString))
			{
				for (int i = 0; i < 3; i++)
				{
					bool isSelected = currentBodyTypeString == bodyTypeStrings[i];
					if (ImGui::Selectable(bodyTypeStrings[i], isSelected))
					{
						currentBodyTypeString = bodyTypeStrings[i];
						component.Type = ((Rigidbody2DComponent::BodyType)i);
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
		});

		DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
		{
			ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
			ImGui::DragFloat2("Size", glm::value_ptr(component.Size));

			ImGui::DragFloat("Density", &component.Density);
			ImGui::DragFloat("Friction", &component.Friction);
			ImGui::DragFloat("Restitution", &component.Restitution);
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
