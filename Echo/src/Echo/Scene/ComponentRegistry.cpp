#include "pch.h"
#include "ComponentRegistry.h"

#include "Entity.h"

#include "AssetManager/AssetRegistry.h"
#include "AssetManager/Assets/TextureAsset.h"
#include "AssetManager/Assets/MeshAsset.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>


namespace YAML
{

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			return node;
		}
		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			return node;
		}
		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;
			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace Echo
{

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		out << v.x << v.y << v.z;
		out << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		out << v.x << v.y << v.z << v.w;
		out << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec2& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq;
		out << v.x << v.y;
		out << YAML::EndSeq;
		return out;
	}

	std::unordered_map<std::type_index, ComponentMetadata> ComponentRegistry::s_ComponentRegistry;

	// Helper function for drawing Vec3 controls (copied from EntityComponentPanel)
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

	// Helper function for drawing component UI frames
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

			bool removeComponent = false;
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

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

	template<typename T>
	void ComponentRegistry::RegisterComponent(const std::string& name, const std::string& category, bool showInAddMenu, bool serializable, bool hasUI)
	{
		ComponentMetadata meta;
		meta.Name = name;
		meta.Category = category;
		meta.ShowInAddMenu = showInAddMenu;
		meta.Serializable = serializable;
		meta.HasUI = hasUI;

		meta.AddComponent = [](Entity& entity)
		{
			if (!entity.HasComponent<T>())
			{
				entity.AddComponent<T>();
			}
		};

		meta.HasComponent = [](const Entity& entity)
		{
			return entity.HasComponent<T>();
		};

		meta.RemoveComponent = [](Entity& entity)
		{
			if (entity.HasComponent<T>())
			{
				entity.RemoveComponent<T>();
			}
		};

		meta.CopyComponent = [](const Entity& source, Entity& destination)
		{
			if (source.HasComponent<T>())
			{
				auto& srcComponent = source.GetComponent<T>();
				destination.AddComponent<T>(srcComponent);
			}
		};

		meta.Serialize = [](const Entity& entity, YAML::Emitter& out)
		{
		};

		meta.Deserialize = [](Entity& entity, const YAML::Node& node)
		{
		};

		meta.DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
		{
		};

		meta.InitializeComponent = [](Entity& entity, void* comp)
		{
		};

		s_ComponentRegistry[GetTypeIndex<T>()] = meta;
	}

	const std::unordered_map<std::type_index, ComponentMetadata>& ComponentRegistry::GetRegisteredComponents()
	{
		return s_ComponentRegistry;
	}

	std::vector<ComponentMetadata*> ComponentRegistry::GetComponentsByCategory(const std::string& category)
	{
		std::vector<ComponentMetadata*> components;
		for (auto& [typeIndex, meta] : s_ComponentRegistry)
		{
			if (meta.Category == category)
			{
				components.push_back(&meta);
			}
		}
		return components;
	}

	std::vector<std::string> ComponentRegistry::GetCategories()
	{
		std::set<std::string> uniqueCategories;
		for (const auto& [typeIndex, meta] : s_ComponentRegistry)
		{
			uniqueCategories.insert(meta.Category);
		}
		return std::vector<std::string>(uniqueCategories.begin(), uniqueCategories.end());
	}

	void ComponentRegistry::SerializeEntity(const Entity& entity, YAML::Emitter& out)
	{
		for (const auto& [typeIndex, meta] : s_ComponentRegistry)
		{
			if (meta.Serializable && meta.HasComponent(entity))
			{
				meta.Serialize(entity, out);
			}
		}
	}

	void ComponentRegistry::DeserializeEntity(Entity& entity, const YAML::Node& entityNode)
	{
		for (const auto& [typeIndex, meta] : s_ComponentRegistry)
		{
			if (meta.Serializable)
			{
				meta.Deserialize(entity, entityNode);
			}
		}
	}

	void ComponentRegistry::DrawEntityUI(Entity& entity, const std::filesystem::path& currentDirectory)
	{
		for (const auto& [typeIndex, meta] : s_ComponentRegistry)
		{
			if (meta.HasUI && meta.HasComponent(entity))
			{
				meta.DrawUI(entity, currentDirectory);
			}
		}
	}

	void ComponentRegistry::DrawAddComponentMenu(Entity& entity)
	{
		auto categories = GetCategories();

		for (const auto& category : categories)
		{
			auto components = GetComponentsByCategory(category);
			bool hasComponentsInCategory = false;

			// Check if there are any components in this category that can be added
			for (const auto& meta : components)
			{
				if (meta->ShowInAddMenu && !meta->HasComponent(entity))
				{
					hasComponentsInCategory = true;
					break;
				}
			}

			if (!hasComponentsInCategory) continue;

			if (categories.size() > 1) // Only show category headers if we have multiple categories
			{
				ImGui::SeparatorText(category.c_str());
			}

			for (const auto& meta : components)
			{
				if (meta->ShowInAddMenu && !meta->HasComponent(entity))
				{
					if (ImGui::MenuItem(meta->Name.c_str()))
					{
						meta->AddComponent(entity);
						ImGui::CloseCurrentPopup();
					}
				}
			}
		}
	}

	void ComponentRegistry::CopyAllComponents(const Entity& source, Entity& destination)
	{
		for (const auto& [typeIndex, meta] : s_ComponentRegistry)
		{
			meta.CopyComponent(source, destination);
		}
	}

	template<typename T>
	bool ComponentRegistry::IsRegistered()
	{
		return s_ComponentRegistry.find(GetTypeIndex<T>()) != s_ComponentRegistry.end();
	}

	template<typename T>
	ComponentMetadata* ComponentRegistry::GetMetadata()
	{
		auto it = s_ComponentRegistry.find(GetTypeIndex<T>());
		return (it != s_ComponentRegistry.end()) ? &it->second : nullptr;
	}

	// Component-specific serialization and UI implementations
	void ComponentRegistry::InitializeComponentRegistry()
	{
		// Register core components
		RegisterComponent<IDComponent>("ID", "Core", false, false, false);
		RegisterComponent<TagComponent>("Tag", "Core", false, true, false);
		RegisterComponent<TransformComponent>("Transform", "Core", false, true, true);

		// Register rendering 2D components
		RegisterComponent<SpriteRendererComponent>("Sprite Renderer", "Rendering 2D");
		RegisterComponent<CircleRendererComponent>("Circle Renderer", "Rendering 2D");
		RegisterComponent<CameraComponent>("Camera", "Rendering 2D");

		// Register rendering 3D components
		RegisterComponent<MeshFilterComponent>("Mesh Filter", "Rendering 3D");

		// Register physics components
		RegisterComponent<Rigidbody2DComponent>("Rigidbody 2D", "Physics");
		RegisterComponent<BoxCollider2DComponent>("Box Collider 2D", "Physics");
		RegisterComponent<CircleCollider2DComponent>("Circle Collider 2D", "Physics");

		// Register scripting components
		RegisterComponent<NativeScriptComponent>("Native Script", "Scripting");

		// Set up specialized serialize/deserialize/UI functions
		SetupComponentSerializers();
		SetupComponentUI();
		SetupComponentInitializers();
	}

	// Specialized serialization setup
	void ComponentRegistry::SetupComponentSerializers()
	{
		// TagComponent
		if (auto meta = ComponentRegistry::GetMetadata<TagComponent>())
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<TagComponent>())
				{
					out << YAML::Key << "TagComponent";
					out << YAML::BeginMap;
					auto& tag = entity.GetComponent<TagComponent>().Tag;
					out << YAML::Key << "Tag" << YAML::Value << tag;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto tagComponent = entityNode["TagComponent"];
				if (tagComponent)
				{
					auto& tc = entity.GetComponent<TagComponent>();
					tc.Tag = tagComponent["Tag"].as<std::string>();
				}
			};
		}

		// TransformComponent
		if (auto meta = ComponentRegistry::GetMetadata<TransformComponent>())
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<TransformComponent>())
				{
					out << YAML::Key << "TransformComponent";
					out << YAML::BeginMap;
					auto& tc = entity.GetComponent<TransformComponent>();
					out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
					out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
					out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto transformComponent = entityNode["TransformComponent"];
				if (transformComponent)
				{
					auto& tc = entity.GetComponent<TransformComponent>();
					tc.Translation = transformComponent["Translation"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}
			};
		}

		// SpriteRendererComponent
		if (auto meta = ComponentRegistry::GetMetadata<SpriteRendererComponent>())
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<SpriteRendererComponent>())
				{
					out << YAML::Key << "SpriteRendererComponent";
					out << YAML::BeginMap;
					auto& spriteRenderer = entity.GetComponent<SpriteRendererComponent>();
					out << YAML::Key << "Color" << YAML::Value << spriteRenderer.Color;
					std::string texturePath = spriteRenderer.Texture ? spriteRenderer.Texture->GetMetadata().Path.string() : "";
					out << YAML::Key << "Texture" << YAML::Value << texturePath;
					out << YAML::Key << "TilingFactor" << YAML::Value << spriteRenderer.TilingFactor;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto spriteRendererComponent = entityNode["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& spriteRenderer = entity.AddComponent<SpriteRendererComponent>();
					spriteRenderer.Color = spriteRendererComponent["Color"].as<glm::vec4>();
					std::string texturePath = spriteRendererComponent["Texture"].as<std::string>();
					spriteRenderer.Texture = texturePath != "" ? AssetRegistry::LoadAsset<TextureAsset>(texturePath) : nullptr;
					if (spriteRendererComponent["TilingFactor"])
						spriteRenderer.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
				}
			};
		}

		if (auto meta = ComponentRegistry::GetMetadata<CircleRendererComponent>())
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<CircleRendererComponent>())
				{
					out << YAML::Key << "CircleRendererComponent";
					out << YAML::BeginMap;
					auto& circleRenderer = entity.GetComponent<CircleRendererComponent>();
					out << YAML::Key << "Color" << YAML::Value << circleRenderer.Color;
					out << YAML::Key << "Thickness" << YAML::Value << circleRenderer.OutlineThickness;
					out << YAML::Key << "Fade" << YAML::Value << circleRenderer.Fade;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto circleRendererComponent = entityNode["CircleRendererComponent"];
				if (circleRendererComponent)
				{
					auto& circleRenderer = entity.AddComponent<CircleRendererComponent>();
					circleRenderer.Color = circleRendererComponent["Color"].as<glm::vec4>();
					circleRenderer.OutlineThickness = circleRendererComponent["Thickness"].as<float>();
					circleRenderer.Fade = circleRendererComponent["Fade"].as<float>();
				}
			};
		}

		if (auto meta = ComponentRegistry::GetMetadata<MeshFilterComponent>())
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<MeshFilterComponent>())
				{
					out << YAML::Key << "MeshFilterComponent";
					out << YAML::BeginMap;
					auto& filter = entity.GetComponent<MeshFilterComponent>();
					std::string meshPath = filter.Mesh ? filter.Mesh->GetMetadata().Path.string() : "";
					out << YAML::Key << "Filter" << YAML::Value << meshPath;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto meshFilterComponent = entityNode["MeshFilterComponent"];
				if (meshFilterComponent)
				{
					auto& meshFilter = entity.AddComponent<MeshFilterComponent>();
					std::string meshPath = meshFilterComponent["Filter"].as<std::string>();
					meshFilter.Mesh = meshPath != "" ? AssetRegistry::LoadAsset<MeshAsset>(meshPath) : nullptr;
				}
			};
		}

		// CameraComponent
		if (auto meta = ComponentRegistry::GetMetadata<CameraComponent>())
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<CameraComponent>())
				{
					out << YAML::Key << "CameraComponent";
					auto& cameraComponent = entity.GetComponent<CameraComponent>();
					out << YAML::BeginMap;
					out << YAML::Key << "Camera" << YAML::Value;
					out << YAML::BeginMap;
					out << YAML::Key << "ProjectionType" << YAML::Value << (int)cameraComponent.Camera.GetProjectionType();
					out << YAML::Key << "PerspectiveFOV" << YAML::Value << cameraComponent.Camera.GetPerspectiveFOV();
					out << YAML::Key << "PerspectiveNear" << YAML::Value << cameraComponent.Camera.GetPerspectiveNearClip();
					out << YAML::Key << "PerspectiveFar" << YAML::Value << cameraComponent.Camera.GetPerspectiveFarClip();
					out << YAML::Key << "OrthographicSize" << YAML::Value << cameraComponent.Camera.GetOrthographicSize();
					out << YAML::Key << "OrthographicNear" << YAML::Value << cameraComponent.Camera.GetOrthographicNearClip();
					out << YAML::Key << "OrthographicFar" << YAML::Value << cameraComponent.Camera.GetOrthographicFarClip();
					out << YAML::EndMap;
					out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
					out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto cameraComponent = entityNode["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = entity.AddComponent<CameraComponent>();
					auto& camera = cc.Camera;
					auto cameraProps = cameraComponent["Camera"];
					camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());
					camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
					camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());
					camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());
					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}
			};
		}

		if (auto meta = ComponentRegistry::GetMetadata<Rigidbody2DComponent>()) 
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<Rigidbody2DComponent>())
				{
					out << YAML::Key << "Rigidbody2DComponent";
					out << YAML::BeginMap;
					auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
					out << YAML::Key << "BodyType" << YAML::Value << (int)rb2d.Type;
					out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.FixedRotation;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto rigidBody2DComponent = entityNode["Rigidbody2DComponent"];
				if (rigidBody2DComponent)
				{
					auto& rb2d = entity.AddComponent<Rigidbody2DComponent>();
					rb2d.Type = (Rigidbody2DComponent::BodyType)rigidBody2DComponent["BodyType"].as<int>();
					rb2d.FixedRotation = rigidBody2DComponent["FixedRotation"].as<bool>();
				}
			};
		}

		if (auto meta = ComponentRegistry::GetMetadata<BoxCollider2DComponent>())
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<BoxCollider2DComponent>())
				{
					out << YAML::Key << "BoxCollider2DComponent";
					out << YAML::BeginMap;
					auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();
					out << YAML::Key << "Offset" << YAML::Value << bc2d.Offset;
					out << YAML::Key << "Size" << YAML::Value << bc2d.Size;
					out << YAML::Key << "Density" << YAML::Value << bc2d.Density;
					out << YAML::Key << "Friction" << YAML::Value << bc2d.Friction;
					out << YAML::Key << "Restitution" << YAML::Value << bc2d.Restitution;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto boxCollider2DComponent = entityNode["BoxCollider2DComponent"];
				if (boxCollider2DComponent)
				{
					auto& bc2d = entity.AddComponent<BoxCollider2DComponent>();
					bc2d.Offset = boxCollider2DComponent["Offset"].as<glm::vec2>();
					bc2d.Size = boxCollider2DComponent["Size"].as<glm::vec2>();
					bc2d.Density = boxCollider2DComponent["Density"].as<float>();
					bc2d.Friction = boxCollider2DComponent["Friction"].as<float>();
					bc2d.Restitution = boxCollider2DComponent["Restitution"].as<float>();
				}
			};
		}

		if (auto meta = ComponentRegistry::GetMetadata<CircleCollider2DComponent>())
		{
			meta->Serialize = [](const Entity& entity, YAML::Emitter& out)
			{
				if (entity.HasComponent<CircleCollider2DComponent>())
				{
					out << YAML::Key << "CircleCollider2DComponent";
					out << YAML::BeginMap;
					auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();
					out << YAML::Key << "Offset" << YAML::Value << cc2d.Offset;
					out << YAML::Key << "Radius" << YAML::Value << cc2d.Radius;
					out << YAML::Key << "Density" << YAML::Value << cc2d.Density;
					out << YAML::Key << "Friction" << YAML::Value << cc2d.Friction;
					out << YAML::Key << "Restitution" << YAML::Value << cc2d.Restitution;
					out << YAML::EndMap;
				}
			};

			meta->Deserialize = [](Entity& entity, const YAML::Node& entityNode)
			{
				auto circleCollider2DComponent = entityNode["CircleCollider2DComponent"];
				if (circleCollider2DComponent)
				{
					auto& cc2d = entity.AddComponent<CircleCollider2DComponent>();
					cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
					cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
					cc2d.Density = circleCollider2DComponent["Density"].as<float>();
					cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
					cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
				}
			};
		}
	}

	// Specialized UI setup
	void ComponentRegistry::SetupComponentUI()
	{
		// TransformComponent UI
		if (auto meta = ComponentRegistry::GetMetadata<TransformComponent>())
		{
			meta->DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
			{
				DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
				{
					DrawVec3Control("Translation", component.Translation);
					glm::vec3 rotation = glm::degrees(component.Rotation);
					DrawVec3Control("Rotation", rotation);
					component.Rotation = glm::radians(rotation);
					DrawVec3Control("Scale", component.Scale, 1.0f);
				});
			};
		}

		// SpriteRendererComponent UI
		if (auto meta = ComponentRegistry::GetMetadata<SpriteRendererComponent>())
		{
			meta->DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
			{
				DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [&currentDirectory](auto& component)
				{
					ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
					ImGui::Button("Texture", ImVec2(100.0f, 0));

					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path texturePath = currentDirectory / path;
							component.Texture = AssetRegistry::LoadAsset<TextureAsset>(texturePath.string());
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.0f);
				});
			};
		}

		if (auto meta = ComponentRegistry::GetMetadata<MeshFilterComponent>())
		{
			meta->DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
			{
				DrawComponent<MeshFilterComponent>("Mesh Filter", entity, [&currentDirectory](auto& component)
				{
					ImGui::Button("Mesh", ImVec2(100.0f, 0));
					if (ImGui::BeginDragDropTarget())
					{
						if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
						{
							const wchar_t* path = (const wchar_t*)payload->Data;
							std::filesystem::path meshPath = currentDirectory / path;
							component.Mesh = AssetRegistry::LoadAsset<MeshAsset>(meshPath.string());
						}
						ImGui::EndDragDropTarget();
					}

					ImGui::SameLine();
					ImGui::Text(component.Mesh->GetMetadata().Path.stem().string().c_str());
				});
			};
		}

		if (auto meta = ComponentRegistry::GetMetadata<CircleRendererComponent>())
		{
			meta->DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
			{
				DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](auto& component)
				{
					ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
					ImGui::DragFloat("Outline Thickness", &component.OutlineThickness, 0.1f, 0.0f, 1.0f);
					ImGui::DragFloat("Fade", &component.Fade, 0.00025f, 0.0f, 0.5f);
				});
			};
		}

		// CameraComponent UI
		if (auto meta = ComponentRegistry::GetMetadata<CameraComponent>())
		{
			meta->DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
			{
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
							component.Camera.SetPerspectiveFOV(glm::radians(perspectiveFOV));

						float perspectiveNear = component.Camera.GetPerspectiveNearClip();
						if (ImGui::DragFloat("Near Clip", &perspectiveNear))
							component.Camera.SetPerspectiveNearClip(perspectiveNear);

						float perspectiveFar = component.Camera.GetPerspectiveFarClip();
						if (ImGui::DragFloat("Far Clip", &perspectiveFar))
							component.Camera.SetPerspectiveFarClip(perspectiveFar);
					}
					else
					{
						float orthographicSize = component.Camera.GetOrthographicSize();
						if (ImGui::DragFloat("Size", &orthographicSize))
							component.Camera.SetOrthographicSize(orthographicSize);

						float orthographicNear = component.Camera.GetOrthographicNearClip();
						if (ImGui::DragFloat("Near Clip", &orthographicNear))
							component.Camera.SetOrthographicNearClip(orthographicNear);

						float orthographicFar = component.Camera.GetOrthographicFarClip();
						if (ImGui::DragFloat("Far Clip", &orthographicFar))
							component.Camera.SetOrthographicFarClip(orthographicFar);
					}

					ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
				});
			};

			if (auto meta = ComponentRegistry::GetMetadata<Rigidbody2DComponent>()) 
			{
				meta->DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
				{
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
				};
			}

			if (auto meta = ComponentRegistry::GetMetadata<BoxCollider2DComponent>()) 
			{
				meta->DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
				{
					DrawComponent<BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& component)
					{
						ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
						ImGui::DragFloat2("Size", glm::value_ptr(component.Size));

						ImGui::DragFloat("Density", &component.Density);
						ImGui::DragFloat("Friction", &component.Friction);
						ImGui::DragFloat("Restitution", &component.Restitution);
					});
				};
			}

			if (auto meta = ComponentRegistry::GetMetadata<CircleCollider2DComponent>())
			{
				meta->DrawUI = [](Entity& entity, const std::filesystem::path& currentDirectory)
				{
					DrawComponent<CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& component)
					{
						ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
						ImGui::DragFloat("Radius", &component.Radius);

						ImGui::DragFloat("Density", &component.Density);
						ImGui::DragFloat("Friction", &component.Friction);
						ImGui::DragFloat("Restitution", &component.Restitution);
					});
				};
			}
		}
		
	}

	void ComponentRegistry::SetupComponentInitializers()
	{
		if (auto meta = GetMetadata<CameraComponent>())
		{
			meta->InitializeComponent = [](Entity& entity, void* comp)
			{
				auto* component = static_cast<CameraComponent*>(comp);
				auto* scene = entity.GetScene();
				component->Camera.SetViewportSize(scene->GetViewportWidth(), scene->GetViewportHeight());
			};
		}
	}

}