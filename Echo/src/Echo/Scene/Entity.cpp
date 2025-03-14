#include "pch.h"

#include "Entity.h"
#include "Components.h"

namespace Echo 
{

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene) {}


}