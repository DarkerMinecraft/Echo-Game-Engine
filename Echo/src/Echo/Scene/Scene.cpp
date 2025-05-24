#include "pch.h"
#include "Scene.h"

#include "Components.h"

#include "Graphics/NamedRenderer/Renderer2D.h"

#include "Entity.h"
#include "ScriptableEntity.h"

#include "Physics/Physics2D.h"

namespace Echo
{

	static b2BodyType Rigidbody2DTypeToBox2DBody(Rigidbody2DComponent::BodyType bodyType)
	{
		switch (bodyType)
		{
			case Rigidbody2DComponent::BodyType::Kinematic: return b2_kinematicBody;
			case Rigidbody2DComponent::BodyType::Dynamic: return b2_dynamicBody;
			case Rigidbody2DComponent::BodyType::Static: return b2_staticBody;
		}
	}

	Scene::Scene()
		: m_Physics2D(CreateScope<Physics2D>())
	{

	}

	Scene::~Scene()
	{

	}

	template<typename T>
	static void CopyComponent(entt::registry& srcRegistry, entt::registry& dstRegistry, std::unordered_map<UUID, entt::entity>& enttMap)
	{
		auto view = srcRegistry.view<IDComponent>();
		for (auto& e : view)
		{
			if (srcRegistry.any_of<T>(e))
			{
				auto& component = srcRegistry.get<T>(e);
				entt::entity dstEntity = enttMap.at(srcRegistry.get<IDComponent>(e).ID);

				dstRegistry.emplace_or_replace<T>(dstEntity, component);
			}
		}
	}

	Ref<Scene> Scene::Copy(Ref<Scene> srcScene)
	{
		EC_PROFILE_FUNCTION();
		Ref<Scene> newScene = CreateRef<Scene>();

		newScene->m_ViewportWidth = srcScene->m_ViewportWidth;
		newScene->m_ViewportHeight = srcScene->m_ViewportHeight;

		std::unordered_map<UUID, entt::entity> enttMap;
		auto& srcRegistry = srcScene->m_Registry;
		auto& dstRegistry = newScene->m_Registry;
		auto view = srcRegistry.view<IDComponent>();
		for (auto& e : view)
		{
			UUID uuid = srcRegistry.get<IDComponent>(e).ID;
			std::string name = srcRegistry.get<TagComponent>(e).Tag;

			enttMap[uuid] = newScene->CreateEntity(name, uuid);
		}

		CopyComponent<TransformComponent>(srcRegistry, dstRegistry, enttMap);
		CopyComponent<SpriteRendererComponent>(srcRegistry, dstRegistry, enttMap);
		CopyComponent<CircleRendererComponent>(srcRegistry, dstRegistry, enttMap);
		CopyComponent<CameraComponent>(srcRegistry, dstRegistry, enttMap);
		CopyComponent<MeshComponent>(srcRegistry, dstRegistry, enttMap);
		CopyComponent<NativeScriptComponent>(srcRegistry, dstRegistry, enttMap);
		CopyComponent<Rigidbody2DComponent>(srcRegistry, dstRegistry, enttMap);
		CopyComponent<BoxCollider2DComponent>(srcRegistry, dstRegistry, enttMap);

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>();
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Unnamed Entity" : name;

		return entity;
	}

	Entity Scene::CreateEntity(const std::string& name, uint64_t uuid)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>(UUID(uuid));
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Unnamed Entity" : name;

		return entity;
	}

	Entity Scene::GetPrimaryCameraEntity()
	{
		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			const auto camera = view.get<CameraComponent>(entity);
			if (camera.Primary)
			{
				return Entity{ entity, this };
			}
		}

		return {  };
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}

	void Scene::OnRuntimeStart()
	{
		EC_PROFILE_FUNCTION();
		b2WorldDef worldDef = b2DefaultWorldDef();
		worldDef.gravity.y = -9.89f;
		worldDef.gravity.x = 0.0f;
		m_Physics2D->StartPhysicsWorld(&worldDef);

		auto view = m_Registry.view<Rigidbody2DComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& transform = entity.GetComponent<TransformComponent>();
			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

			b2BodyDef bodyDef = b2DefaultBodyDef();
			bodyDef.type = Rigidbody2DTypeToBox2DBody(rb2d.Type);
			bodyDef.position.x = transform.Translation.x;
			bodyDef.position.y = transform.Translation.y;
			bodyDef.rotation = b2MakeRot(transform.Rotation.z);
			bodyDef.fixedRotation = rb2d.FixedRotation;

			b2BodyId body = m_Physics2D->AddBody(entity.GetUUID(), &bodyDef);

			if (entity.HasComponent<BoxCollider2DComponent>())
			{
				auto& bc2d = entity.GetComponent<BoxCollider2DComponent>();

				b2Polygon boxShape = b2MakeBox(transform.Scale.x * bc2d.Size.x, transform.Scale.y * bc2d.Size.y);
				b2ShapeDef shapeDef = b2DefaultShapeDef();
				shapeDef.density = bc2d.Density;
				shapeDef.material.friction = bc2d.Friction;
				shapeDef.material.restitution = bc2d.Restitution;

				m_Physics2D->AddShape(entity.GetUUID(), body, &shapeDef, &boxShape);
			}
		}
	}

	void Scene::OnRuntimeStop()
	{
		m_Physics2D->EndPhysicsWorld();
	}

	void Scene::OnUpdateEditor(CommandList& cmd, const EditorCamera& camera, Timestep ts)
	{
		EC_PROFILE_FUNCTION();
		Renderer2D::BeginScene(cmd, camera);

		{
			auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawQuad({ .InstanceID = (int)(uint32_t)entity, .Color = sprite.Color, .Texture = sprite.Texture ? sprite.Texture->GetTexture() : nullptr, .TilingFactor = sprite.TilingFactor}, transform.GetTransform());
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
			for (auto entity : view)
			{
				auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
				Renderer2D::DrawCircle({ .InstanceID = (int)(uint32_t)entity, .Color = circle.Color, .OutlineThickness = circle.OutlineThickness, .Fade = circle.Fade }, transform.GetTransform());
			}
		}

		Renderer2D::EndScene();
	}

	void Scene::OnUpdateRuntime(CommandList& cmd, Timestep ts)
	{
		EC_PROFILE_FUNCTION();
		m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
		{
			if (!nsc.Instance)
			{
				nsc.Instance = nsc.InstantiateScript();
				nsc.Instance->m_Entity = Entity{ entity, this };

				nsc.Instance->OnCreate();
			}

			nsc.Instance->OnUpdate(ts);
		});

		Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform;
		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

				if (camera.Primary)
				{
					mainCamera = &camera.Camera;
					cameraTransform = transform.GetTransform();
					break;
				}
			}
		}

		// Physics
		{
			int subStepCount = 4;
			m_Physics2D->Step(1.0f / 60.0f, subStepCount);

			auto view = m_Registry.view<Rigidbody2DComponent>();
			for (auto e : view)
			{
				Entity entity = { e, this };

				auto& transform = entity.GetComponent<TransformComponent>();
				auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();

				const auto& position = m_Physics2D->GetPosition(entity.GetUUID());
				transform.Translation.x = position.x;
				transform.Translation.y = position.y;
				const auto& rotation = m_Physics2D->GetRotation(entity.GetUUID());
				transform.Rotation.z = rotation;
			}
		}

		if (mainCamera != nullptr)
		{
			Renderer2D::BeginScene(cmd, *mainCamera, cameraTransform);

			{
				auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer2D::DrawQuad({ .InstanceID = (int)(uint32_t)entity, .Color = sprite.Color, .Texture = sprite.Texture ? sprite.Texture->GetTexture() : nullptr, .TilingFactor = sprite.TilingFactor}, transform.GetTransform());
				}
			}

			{
				auto view = m_Registry.view<TransformComponent, CircleRendererComponent>();
				for (auto entity : view)
				{
					auto [transform, circle] = view.get<TransformComponent, CircleRendererComponent>(entity);
					Renderer2D::DrawCircle({ .InstanceID = (int)(uint32_t)entity, .Color = circle.Color, .OutlineThickness = circle.OutlineThickness, .Fade = circle.Fade }, transform.GetTransform());
				}
			}

			Renderer2D::EndScene();
		}
	}

	void Scene::OnViewportResize(uint32_t width, uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		auto view = m_Registry.view<CameraComponent>();
		for (auto entity : view)
		{
			auto& cameraComponent = view.get<CameraComponent>(entity);
			if (!cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}

	template<typename T>
	void Scene::OnComponentAdd(Entity entity, T& component)
	{
		//static_assert(false);
	}

	template<>
	void Scene::OnComponentAdd<IDComponent>(Entity entity, IDComponent& component)
	{

	}


	template<>
	void Scene::OnComponentAdd<TransformComponent>(Entity entity, TransformComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdd<CameraComponent>(Entity entity, CameraComponent& component)
	{
		component.Camera.SetViewportSize(m_ViewportWidth, m_ViewportHeight);
	}

	template<>
	void Scene::OnComponentAdd<SpriteRendererComponent>(Entity entity, SpriteRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdd<CircleRendererComponent>(Entity entity, CircleRendererComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdd<NativeScriptComponent>(Entity entity, NativeScriptComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdd<TagComponent>(Entity entity, TagComponent& component)
	{

	}


	template<>
	void Scene::OnComponentAdd<MeshComponent>(Entity entity, MeshComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdd<Rigidbody2DComponent>(Entity entity, Rigidbody2DComponent& component)
	{

	}

	template<>
	void Scene::OnComponentAdd<BoxCollider2DComponent>(Entity entity, BoxCollider2DComponent& component)
	{

	}

}