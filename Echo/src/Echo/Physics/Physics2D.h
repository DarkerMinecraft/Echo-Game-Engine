#pragma once

#include "Core/UUID.h"

#include <box2d/box2d.h>
#include <glm/glm.hpp>

namespace Echo 
{

	struct EntityPhysics 
	{
		b2BodyId Body;
		b2Polygon* PolygonShape;
	};

	class Physics2D 
	{
	public:
		Physics2D() = default;
		~Physics2D() = default;

		void StartPhysicsWorld(b2WorldDef* worldDef);

		void Step(float timeStep, int subStepCount);

		b2BodyId AddBody(UUID uuid, b2BodyDef* bodyDef);
		void AddShape(UUID uuid, b2BodyId bodyId, b2ShapeDef* shapeDef, b2Polygon* polygon);

		glm::vec2 GetPosition(UUID uuid);
		float GetRotation(UUID uuid);

		void EndPhysicsWorld();
	private:
		b2WorldId m_PhysicsWorld;

		std::unordered_map<UUID, EntityPhysics> m_EntitiesPhysics;
	};
}