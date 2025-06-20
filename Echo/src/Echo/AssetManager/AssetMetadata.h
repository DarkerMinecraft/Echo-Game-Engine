#pragma once

#include "Core/UUID.h"

#include <glm/glm.hpp>

#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>

namespace Echo 
{

	enum class AssetType
	{
		None = 0,

		Shader,
		Texture,
		Material,
		Scene,
		Mesh
	};

	using AssetVariant = std::variant<
		bool, int, float, double, std::string,
		glm::vec2, glm::vec3, glm::vec4,
		glm::mat4, UUID
	>;

	struct AssetMetadata
	{
		UUID ID;
		AssetType Type;
		std::filesystem::path Path;
		std::filesystem::file_time_type LastModified;
		std::unordered_map<std::string, AssetVariant> CustomProps; 

		bool SerializeToFile(const std::filesystem::path& filepath);
		bool DeserializeFromFile(const std::filesystem::path& filepath);
	};

}