#include "pch.h"
#include "SceneSerializer.h"

#include "Scene/Entity.h"
#include "Scene/Components.h"
#include "Scene/ComponentRegistry.h"

#include "AssetManager/AssetRegistry.h"
#include "AssetManager/Assets/TextureAsset.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

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
	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		EC_PROFILE_FUNCTION();

		out << YAML::BeginMap;
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		ComponentRegistry::SerializeEntity(entity, out);

		out << YAML::EndMap;
	}

	void SceneSerializer::Serialize(const std::string& filepath)
	{
		EC_PROFILE_FUNCTION();

		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Entities";
		out << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.view<entt::entity>().each([&](auto entityID)
		{
			Entity entity = { entityID, m_Scene.get() };
			if (!entity)
				return;

			SerializeEntity(out, entity);
		});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	void SceneSerializer::SerializeRuntime(const std::string& filepath)
	{

	}

	bool SceneSerializer::Deserialize(const std::string& filepath)
	{
		EC_PROFILE_FUNCTION();

		std::ifstream stream(filepath);
		if (!stream.is_open())
		{
			EC_CORE_ERROR("Could not open file '{0}'", filepath);
			return false;
		}
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t id = entity["Entity"].as<uint64_t>();

				auto tagComponent = entity["TagComponent"];
				std::string name;
				if (tagComponent)
				{
					name = tagComponent["Tag"].as<std::string>();
				}

				Entity deserializedEntity = m_Scene->CreateEntity(name, id);
				ComponentRegistry::DeserializeEntity(deserializedEntity, entity);
			}

			return true;
		}
		
		return false;
	}

	bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
	{
		return false;
	}

}