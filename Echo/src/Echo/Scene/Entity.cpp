#include "pch.h"

#include "Entity.h"
#include "Components.h"

namespace Echo
{

	Entity::Entity(entt::entity handle, Scene* scene)
		: m_EntityHandle(handle), m_Scene(scene)
	{}


	void Entity::SetComponentOrder(const std::vector<std::string>& order)
	{
		if (!HasComponent<ComponentOrderComponent>())
		{
			AddComponent<ComponentOrderComponent>().ComponentOrder = order;
		}
		GetComponent<ComponentOrderComponent>().ComponentOrder = order;
	}

	std::vector<std::string> Entity::GetComponentOrder()
	{
		if (!HasComponent<ComponentOrderComponent>())
		{
            // Create default order if component doesn't exist
			std::vector<std::string> defaultOrder;

			// Preferred logical order
			const std::vector<std::string> preferredOrder = {
				"Transform", "Sprite Renderer", "Circle Renderer", "Camera",
				"Mesh Filter", "Rigidbody 2D", "Box Collider 2D", "Circle Collider 2D",
				"Native Script"
			};

			// Add existing components in preferred order
			for (const std::string& componentName : preferredOrder)
			{
				auto* meta = ComponentRegistry::GetMetadataByName(componentName);
				if (meta && meta->HasComponent(*this) && meta->HasUI)
				{
					defaultOrder.push_back(componentName);
				}
			}

			// Add any remaining components not in preferred order
			for (const auto& [typeIndex, meta] : ComponentRegistry::GetRegisteredComponents())
			{
				if (meta.HasComponent(*this) && meta.HasUI)
				{
					if (std::find(defaultOrder.begin(), defaultOrder.end(), meta.Name) == defaultOrder.end())
					{
						defaultOrder.push_back(meta.Name);
					}
				}
			}

			AddComponent<ComponentOrderComponent>().ComponentOrder = defaultOrder;
			return defaultOrder;
		}

		return GetComponent<ComponentOrderComponent>().ComponentOrder;
	}

	void Entity::ReorderComponent(const std::string& componentName, size_t newIndex)
	{
		if (!HasComponent<ComponentOrderComponent>())
		{
			GetComponentOrder(); // Initialize with defaults
		}

		auto& order = GetComponent<ComponentOrderComponent>().ComponentOrder;

		// Find the component in current order
		auto it = std::find(order.begin(), order.end(), componentName);
		if (it == order.end())
		{
			// Component not in order list, add it at specified index
			if (newIndex >= order.size())
			{
				order.push_back(componentName);
			}
			else
			{
				order.insert(order.begin() + newIndex, componentName);
			}
			return;
		}

		// Remove from current position
		order.erase(it);

		// Clamp newIndex to valid range
		newIndex = std::min(newIndex, order.size());

		// Insert at new position
		order.insert(order.begin() + newIndex, componentName);
	}
}