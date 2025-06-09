#pragma once

#include "Components.h"

#include <yaml-cpp/yaml.h>
#include <functional>
#include <unordered_map>
#include <typeindex>
#include <string>
#include <vector>

namespace Echo
{
	// Forward declarations
	class Entity;

	// Component operation function types
	using AddComponentFunc = std::function<void(Entity&)>;
	using HasComponentFunc = std::function<bool(const Entity&)>;
	using RemoveComponentFunc = std::function<void(Entity&)>;
	using SerializeFunc = std::function<void(const Entity&, YAML::Emitter&)>;
	using DeserializeFunc = std::function<void(Entity&, const YAML::Node&)>;
	using DrawUIFunc = std::function<void(Entity&, const std::filesystem::path&)>;
	using CopyComponentFunc = std::function<void(const Entity&, Entity&)>;
	using InitializeComponentFunc = std::function<void(Entity&, void*)>;

	// Metadata container for each component type
	struct ComponentMetadata
	{
		std::string Name;
		std::string Category = "General"; // For UI grouping

		AddComponentFunc AddComponent;
		HasComponentFunc HasComponent;
		RemoveComponentFunc RemoveComponent;
		SerializeFunc Serialize;
		DeserializeFunc Deserialize;
		DrawUIFunc DrawUI;
		CopyComponentFunc CopyComponent;
		InitializeComponentFunc InitializeComponent;

		bool ShowInAddMenu = true;
		bool Serializable = true;
		bool HasUI = true;
	};

	class ComponentRegistry
	{
	public:
		// Register a component type with all its operations
		template<typename T>
		static void RegisterComponent(const std::string& name,
									  const std::string& category = "General",
									  bool showInAddMenu = true,
									  bool serializable = true,
									  bool hasUI = true);

		  // Get all registered components
		static const std::unordered_map<std::type_index, ComponentMetadata>& GetRegisteredComponents();

		// Get components by category for UI grouping
		static std::vector<ComponentMetadata*> GetComponentsByCategory(const std::string& category);

		// Get all unique categories
		static std::vector<std::string> GetCategories();

		// Component operations
		static void SerializeEntity(const Entity& entity, YAML::Emitter& out);
		static void DeserializeEntity(Entity& entity, const YAML::Node& entityNode);
		static void DrawEntityUI(Entity& entity, const std::filesystem::path& currentDirectory);
		static void DrawAddComponentMenu(Entity& entity);
		static void CopyAllComponents(const Entity& source, Entity& destination);

		// Check if a component type is registered
		template<typename T>
		static bool IsRegistered();

		// Get metadata for a specific component type
		template<typename T>
		static ComponentMetadata* GetMetadata();

		// Initialize all component registrations
		static void InitializeComponentRegistry();
	private:
		static std::unordered_map<std::type_index, ComponentMetadata> s_ComponentRegistry;

		// Helper function to get type index
		template<typename T>
		static std::type_index GetTypeIndex() { return std::type_index(typeid(T)); }

		static void SetupComponentSerializers();
		static void SetupComponentUI();
		static void SetupComponentInitializers();
	};

	// Convenience macro for registering components
#define REGISTER_COMPONENT(ComponentType, DisplayName) \
        static bool registered_##ComponentType = []() { \
            ComponentRegistry::RegisterComponent<ComponentType>(DisplayName); \
            return true; \
        }();

#define REGISTER_COMPONENT_CATEGORY(ComponentType, DisplayName, Category) \
        static bool registered_##ComponentType = []() { \
            ComponentRegistry::RegisterComponent<ComponentType>(DisplayName, Category); \
            return true; \
        }();

#define REGISTER_COMPONENT_FULL(ComponentType, DisplayName, Category, ShowInMenu, Serializable, HasUI) \
        static bool registered_##ComponentType = []() { \
            ComponentRegistry::RegisterComponent<ComponentType>(DisplayName, Category, ShowInMenu, Serializable, HasUI); \
            return true; \
        }();
}