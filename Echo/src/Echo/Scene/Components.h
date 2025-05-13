#pragma once

#include <glm/glm.hpp>

#include "SceneCamera.h"
#include "Graphics/Texture.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"

#include "Core/Timestep.h"
#include "Core/UUID.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Echo 
{

	struct IDComponent 
	{
		UUID ID;

		IDComponent() = default;
		IDComponent(const IDComponent&) = default;
		IDComponent(UUID id) 
			: ID(id)
		{}
	};

	struct TagComponent
	{
		std::string Tag;

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			: Tag(tag)
		{}
	};

	struct TransformComponent 
	{
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const glm::vec3& translation)
			: Translation(translation)
		{}

		glm::mat4 GetTransform() const 
		{
			glm::mat4 translation = glm::translate(glm::mat4(1.0f), Translation);
			glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), Rotation.x, { 1, 0, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.y, { 0, 1, 0 })
				* glm::rotate(glm::mat4(1.0f), Rotation.z, { 0, 0, 1 });
			glm::mat4 scale = glm::scale(glm::mat4(1.0f), Scale);
			return translation * rotation * scale;
		};
	};

	struct SpriteRendererComponent
	{
		glm::vec4 Color{1.0f, 1.0f, 1.0f, 1.0f};
		Ref<Texture2D> Texture = nullptr;
		float TilingFactor = 1.0f;

		SpriteRendererComponent() = default;
		SpriteRendererComponent(const SpriteRendererComponent&) = default;
		SpriteRendererComponent(const glm::vec4& color, Ref<Texture2D> texture = nullptr, float tilingFactor = 1.0f)
			: Color(color), Texture(texture), TilingFactor(tilingFactor)
		{}
		
		operator glm::vec4& () { return Color; }
		operator const glm::vec4& () const { return Color; }

		operator Ref<Texture2D>& () { return Texture; }
		operator const Ref<Texture2D>& () const { return Texture; }

		operator float& () { return TilingFactor; }
		operator const float& () const { return TilingFactor; }
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = false;
		bool FixedAspectRatio = false;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct MeshComponent 
	{
		Ref<Material> Mat;
		Ref<Mesh> Mes;

		MeshComponent() = default;
		MeshComponent(const MeshComponent&) = default;

		MeshComponent(Ref<Mesh> mesh, Ref<Material> material)
			: Mes(mesh), Mat(material)
		{}

		operator Ref<Mesh>& () { return Mes; }
		operator const Ref<Mesh>& () const { return Mes; }

		operator Ref<Material>& () { return Mat; }
		operator const Ref<Material>& () const { return Mat; }
	};

	class ScriptableEntity;
	struct NativeScriptComponent
	{
		ScriptableEntity* Instance = nullptr;

		ScriptableEntity*(*InstantiateScript)();
		void (*DestroyScript)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			InstantiateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
			DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
		}
	};

}