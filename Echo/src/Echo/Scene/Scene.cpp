#include "pch.h"
#include "Scene.h"

#include "Components.h"

#include "Graphics/NamedRenderer/Renderer2D.h"

#include "Entity.h"
#include "ScriptableEntity.h"

#include "Physics/Physics2D.h"
#include "ComponentRegistry.h"

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
		return b2_staticBody; // Default case
	}

	Scene::Scene()
		: m_Physics2D(CreateScope<Physics2D>())
	{
		static bool registryInitialized = false;
		if (!registryInitialized)
		{
			ComponentRegistry::InitializeComponentRegistry();
			registryInitialized = true;
		}
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
		Ref<Scene> newScene = CreateRef<Scene>();

		// Copy scene properties
		newScene->m_ViewportWidth = srcScene->m_ViewportWidth;
		newScene->m_ViewportHeight = srcScene->m_ViewportHeight;

		// Create entity mapping
		std::unordered_map<UUID, Entity> enttMap;
		auto view = srcScene->m_Registry.view<IDComponent>();
		for (auto srcHandle : view)
		{
			UUID uuid = srcScene->m_Registry.get<IDComponent>(srcHandle).ID;
			std::string name = srcScene->m_Registry.get<TagComponent>(srcHandle).Tag;

			// Create in destination registry
			Entity dstEntity = newScene->CreateEntity(name, uuid);
			enttMap[uuid] = dstEntity;
		}

		// Copy all components
		for (auto& [uuid, dstEntity] : enttMap)
		{
			Entity srcEntity = srcScene->GetEntityByUUID(uuid);
			if (srcEntity.GetHandle() != entt::null)
			{
				ComponentRegistry::CopyAllComponents(srcEntity, dstEntity);
			}
		}

		return newScene;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<IDComponent>();
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Unnamed Entity" : name;

		m_EntityDisplayOrder.push_back(entity.GetUUID()); 
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


	Entity Scene::GetEntityByUUID(UUID uuid)
	{
		auto view = m_Registry.view<IDComponent>();
		for (auto entityHandle : view)
		{
			Entity entity{ entityHandle, this };
			if (entity.GetComponent<IDComponent>().ID == uuid)
			{
				return entity;
			}
		}

		// Return invalid entity if not found
		return Entity{ entt::null, this };
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

	void Scene::DuplicateEntity(Entity entity)
	{
		std::string name = entity.GetComponent<TagComponent>().Tag;
		name += " (" + std::to_string(entity.GetDuplicatedNumber()) + ")";

		Entity newEntity = CreateEntity(name);
		ComponentRegistry::CopyAllComponents(entity, newEntity);
		newEntity.GetComponent<TagComponent>().Tag = name;
		newEntity.GetComponent<IDComponent>().ID = UUID();

		entity.AddDuplicatedNumber();
	}

	void Scene::DestroyEntity(Entity entity)
	{
		auto it = std::find(m_EntityDisplayOrder.begin(), m_EntityDisplayOrder.end(), entity.GetUUID());
		if (it != m_EntityDisplayOrder.end())
		{
			m_EntityDisplayOrder.erase(it);
		}
		m_Registry.destroy(entity);
	}

	void Scene::ReorderEntity(UUID entityUUID, size_t newIndex)
	{
		auto it = std::find(m_EntityDisplayOrder.begin(), m_EntityDisplayOrder.end(), entityUUID);
		if (it != m_EntityDisplayOrder.end())
		{
			m_EntityDisplayOrder.erase(it);
			m_EntityDisplayOrder.insert(m_EntityDisplayOrder.begin() + newIndex, entityUUID);
		}
	}

	void Scene::AddEntityToOrder(UUID entityUUID)
	{
		m_EntityDisplayOrder.push_back(entityUUID);
	}

	void Scene::RemoveEntityFromOrder(UUID entityUUID)
	{
		auto it = std::find(m_EntityDisplayOrder.begin(), m_EntityDisplayOrder.end(), entityUUID);
		if (it != m_EntityDisplayOrder.end())
		{
			m_EntityDisplayOrder.erase(it);
		}
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

			if (entity.HasComponent<CircleCollider2DComponent>()) 
			{
				auto& cc2d = entity.GetComponent<CircleCollider2DComponent>();

				b2Circle circleShape;
				circleShape.center = { cc2d.Offset.x, cc2d.Offset.y };
				circleShape.radius = transform.Scale.x * (cc2d.Radius / 2);
				b2ShapeDef shapeDef = b2DefaultShapeDef();
				shapeDef.density = cc2d.Density;
				shapeDef.material.friction = cc2d.Friction;
				shapeDef.material.restitution = cc2d.Restitution;

				m_Physics2D->AddShape(entity.GetUUID(), body, &shapeDef, &circleShape);
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

}