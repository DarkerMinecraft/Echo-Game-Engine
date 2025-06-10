#include "pch.h"
#include "AssetManager/AssetMetadata.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Echo 
{
	bool AssetMetadata::SerializeToFile(const std::filesystem::path& filepath)
	{
		EC_PROFILE_FUNCTION();
		YAML::Emitter out;
		out << YAML::BeginMap;

		// Basic metadata
		out << YAML::Key << "Asset" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "UUID" << YAML::Value << (uint64_t)ID;

		// Serialize asset type as string for readability
		out << YAML::Key << "Type" << YAML::Value;
		switch (Type)
		{
			case AssetType::Shader: out << "Shader"; break;
			case AssetType::Material: out << "Material"; break;
			case AssetType::Scene: out << "Scene"; break;
			case AssetType::Texture: out << "Texture"; break;
			case AssetType::Mesh: out << "Mesh"; break;
		}

		out << YAML::Key << "Path" << YAML::Value << Path.string();

		// Last modified timestamp
		auto timepoint = std::chrono::time_point_cast<std::chrono::seconds>(LastModified);
		auto value = timepoint.time_since_epoch().count();
		out << YAML::Key << "LastModified" << YAML::Value << value;

		// Custom properties
		if (!CustomProps.empty())
		{
			out << YAML::Key << "Properties" << YAML::Value << YAML::BeginMap;

			for (const auto& [key, value] : CustomProps)
			{
				out << YAML::Key << key << YAML::Value;

				// Use std::visit to handle the variant types
				std::visit([&out](auto&& arg)
				{
					using T = std::decay_t<decltype(arg)>;

					if constexpr (std::is_same_v<T, bool>)
						out << YAML::Value << arg;
					else if constexpr (std::is_same_v<T, int>)
						out << YAML::Value << arg;
					else if constexpr (std::is_same_v<T, float>)
						out << YAML::Value << arg;
					else if constexpr (std::is_same_v<T, double>)
						out << YAML::Value << arg;
					else if constexpr (std::is_same_v<T, std::string>)
						out << YAML::Value << arg;
					else if constexpr (std::is_same_v<T, glm::vec2>)
						out << YAML::Flow << YAML::BeginSeq << arg.x << arg.y << YAML::EndSeq;
					else if constexpr (std::is_same_v<T, glm::vec3>)
						out << YAML::Flow << YAML::BeginSeq << arg.x << arg.y << arg.z << YAML::EndSeq;
					else if constexpr (std::is_same_v<T, glm::vec4>)
						out << YAML::Flow << YAML::BeginSeq << arg.x << arg.y << arg.z << arg.w << YAML::EndSeq;
					else if constexpr (std::is_same_v<T, UUID>)
						out << YAML::Value << (uint64_t)arg;
					else
						out << YAML::Value << "Unsupported Type";
				}, value);
			}

			out << YAML::EndMap; // Properties
		}

		out << YAML::EndMap; // Asset
		out << YAML::EndMap; // Root

		// Write to file
		std::ofstream fout(filepath);
		if (!fout)
			return false;

		fout << out.c_str();
		return true;
	}

	bool AssetMetadata::DeserializeFromFile(const std::filesystem::path& filepath)
	{
		EC_PROFILE_FUNCTION();
		if (!std::filesystem::exists(filepath))
			return false;

		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data;
		try
		{
			data = YAML::Load(strStream.str());
		}
		catch (const YAML::ParserException& e)
		{
			EC_CORE_ERROR("Failed to parse asset metadata file {0}: {1}", filepath.string(), e.what());
			return false;
		}

		if (!data["Asset"])
		{
			EC_CORE_ERROR("Invalid asset metadata file: {0}", filepath.string());
			return false;
		}

		auto asset = data["Asset"];

		// Read UUID
		ID = asset["UUID"].as<uint64_t>();

		// Read asset type
		std::string typeStr = asset["Type"].as<std::string>();
		if (typeStr == "Shader")
			Type = AssetType::Shader;
		else if (typeStr == "Material")
			Type = AssetType::Material;
		else if (typeStr == "Scene")
			Type = AssetType::Scene;
		else if (typeStr == "Texture")
			Type = AssetType::Texture;
		else if (typeStr == "Mesh")
			Type = AssetType::Mesh;
		// Add other types here

		// Read path
		Path = asset["Path"].as<std::string>();

		// Read last modified time
		if (asset["LastModified"])
		{
			auto timeValue = asset["LastModified"].as<int64_t>();
			LastModified = std::filesystem::file_time_type(std::chrono::seconds(timeValue));
		}
		else
		{
			// If not in file, use current file's timestamp
			LastModified = std::filesystem::last_write_time(filepath);
		}

		// Read custom properties
		if (asset["Properties"])
		{
			auto props = asset["Properties"];
			for (const auto& prop : props)
			{
				std::string key = prop.first.as<std::string>();
				YAML::Node valueNode = prop.second;

				// Try to determine the type from the YAML node
				if (valueNode.IsScalar())
				{
					// Try different scalar types
					try
					{
						if (valueNode.Tag() == "!") // Boolean tag in YAML
							CustomProps[key] = valueNode.as<bool>();
						else if (valueNode.as<std::string>() == "true" || valueNode.as<std::string>() == "false")
							CustomProps[key] = valueNode.as<bool>();
						else if (valueNode.as<std::string>().find('.') != std::string::npos)
							CustomProps[key] = valueNode.as<double>();
						else
							CustomProps[key] = valueNode.as<int>();
					}
					catch (...)
					{
		   // Default to string if we can't determine
						CustomProps[key] = valueNode.as<std::string>();
					}
				}
				else if (valueNode.IsSequence())
				{
					// Handle vector types based on sequence length
					auto seq = valueNode.as<std::vector<float>>();
					if (seq.size() == 2)
						CustomProps[key] = glm::vec2(seq[0], seq[1]);
					else if (seq.size() == 3)
						CustomProps[key] = glm::vec3(seq[0], seq[1], seq[2]);
					else if (seq.size() == 4)
						CustomProps[key] = glm::vec4(seq[0], seq[1], seq[2], seq[3]);
				}
			}
		}

		return true;
	}
}