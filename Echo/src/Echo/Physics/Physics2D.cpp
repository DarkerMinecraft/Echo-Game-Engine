#include "pch.h"
#include "Physics2D.h"

namespace Echo 
{

	void Physics2D::StartPhysicsWorld(b2WorldDef* worldDef)
	{
		m_PhysicsWorld = b2CreateWorld(worldDef);
	}

	void Physics2D::Step(float timeStep, int subStepCount)
	{
		b2World_Step(m_PhysicsWorld, timeStep, subStepCount);
	}

	b2BodyId Physics2D::AddBody(UUID uuid, b2BodyDef* bodyDef)
	{
		b2BodyId bodyId = b2CreateBody(m_PhysicsWorld, bodyDef);

		if (m_EntitiesPhysics.contains(uuid)) 
		{
			EntityPhysics ep = m_EntitiesPhysics.at(uuid);
			ep.Body = bodyId; 
			m_EntitiesPhysics[uuid] = ep;
		}
		else 
		{
			EntityPhysics ep{};
			ep.Body = bodyId;
			m_EntitiesPhysics[uuid] = ep;
		}

		return bodyId;
	}

	void Physics2D::AddShape(UUID uuid, b2BodyId bodyId, b2ShapeDef* shapeDef, b2Polygon* polygon)
	{
		b2CreatePolygonShape(bodyId, shapeDef, polygon);

		if (m_EntitiesPhysics.contains(uuid))
		{
			EntityPhysics ep = m_EntitiesPhysics.at(uuid);
			ep.PolygonShape = polygon;
			m_EntitiesPhysics[uuid] = ep;
		}
		else
		{
			EntityPhysics ep{};
			ep.PolygonShape = polygon;
			m_EntitiesPhysics[uuid] = ep;
		}
	}

	const glm::vec2& Physics2D::GetPosition(UUID uuid)
	{
		b2Vec2 position = b2Body_GetPosition(m_EntitiesPhysics[uuid].Body);

		return { position.x, position.y };
	}

	const float& Physics2D::GetRotation(UUID uuid)
	{
		b2Rot rotation = b2Body_GetRotation(m_EntitiesPhysics[uuid].Body);

		return b2Rot_GetAngle(rotation);
	}

	void Physics2D::EndPhysicsWorld()
	{
		b2DestroyWorld(m_PhysicsWorld);
	}

}