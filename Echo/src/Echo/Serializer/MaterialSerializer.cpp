#include "pch.h"
#include "Material.h"
#include "Asset/AssetManager.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Echo
{
	// Helpers for YAML serialization of glm types
	namespace YAML
	{
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

	bool Material::SerializeToYAML(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		// Asset metadata
		out << YAML::Key << "Asset" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << "Material";
		out << YAML::Key << "UUID" << YAML::Value << (uint64_t)m_UUID;
		out << YAML::Key << "Name" << YAML::Value << m_Name;
		out << YAML::EndMap;

		// Material data
		out << YAML::Key << "Material" << YAML::Value << YAML::BeginMap;

		// Pipeline reference
		if (m_Pipeline)
		{
			out << YAML::Key << "Pipeline" << YAML::Value << (uint64_t)m_Pipeline->GetUUID();
		}

		// Parameters
		out << YAML::Key << "Parameters" << YAML::Value << YAML::BeginSeq;
		for (const auto& [name, value] : m_Parameters)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Name" << YAML::Value << name;

			// Output the value based on its type
			std::visit([&out](auto&& arg)
			{
				using T = std::decay_t<decltype(arg)>;

				out << YAML::Key << "Type" << YAML::Value;

				if constexpr (std::is_same_v<T, float>)
				{
					out << "Float";
					out << YAML::Key << "Value" << YAML::Value << arg;
				}
				else if constexpr (std::is_same_v<T, int>)
				{
					out << "Int";
					out << YAML::Key << "Value" << YAML::Value << arg;
				}
				else if constexpr (std::is_same_v<T, bool>)
				{
					out << "Bool";
					out << YAML::Key << "Value" << YAML::Value << arg;
				}
				else if constexpr (std::is_same_v<T, glm::vec2>)
				{
					out << "Vector2";
					out << YAML::Key << "Value" << YAML::Value << arg;
				}
				else if constexpr (std::is_same_v<T, glm::vec3>)
				{
					out << "Vector3";
					out << YAML::Key << "Value" << YAML::Value << arg;
				}
				else if constexpr (std::is_same_v<T, glm::vec4>)
				{
					out << "Vector4";
					out << YAML::Key << "Value" << YAML::Value << arg;
				}
				else if constexpr (std::is_same_v<T, glm::mat4>)
				{
					out << "Matrix4";
					// Serialize 4x4 matrix - simplified for example
					out << YAML::Key << "Value" << YAML::Value << YAML::BeginSeq;
					for (int i = 0; i < 4; i++)
					{
						out << YAML::BeginSeq;
						for (int j = 0; j < 4; j++)
							out << arg[i][j];
						out << YAML::EndSeq;
					}
					out << YAML::EndSeq;
				}
				else if constexpr (std::is_same_v<T, UUID>)
				{
					out << "AssetReference";
					out << YAML::Key << "Value" << YAML::Value << (uint64_t)arg;
				}
			}, value);

			// ShaderParameter name mapping
			out << YAML::Key << "ShaderParameterName" << YAML::Value << name; // By default same as param name

			out << YAML::EndMap;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap; // Material
		out << YAML::EndMap; // Root

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	std::pair<std::vector<MaterialParameter>, UUID> Material::DeserializeFromYAML(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath);
		if (!stream.is_open())
		{
			EC_CORE_ERROR("Could not open material file: {0}", filepath.string());
			return { {}, UUID() };
		}

		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Material"])
		{
			EC_CORE_ERROR("Invalid material file: {0}", filepath.string());
			return { {}, UUID() };
		}

		std::vector<MaterialParameter> parameters;
		UUID pipelineUUID;

		auto material = data["Material"];
		if (material["Pipeline"])
		{
			pipelineUUID = material["Pipeline"].as<uint64_t>();
		}

		if (material["Parameters"])
		{
			for (const auto& param : material["Parameters"])
			{
				MaterialParameter parameter;
				parameter.Name = param["Name"].as<std::string>();

				if (param["ShaderParameterName"])
					parameter.ShaderParameterName = param["ShaderParameterName"].as<std::string>();
				else
					parameter.ShaderParameterName = parameter.Name;

				std::string type = param["Type"].as<std::string>();

				if (type == "Float")
				{
					parameter.Value = param["Value"].as<float>();
				}
				else if (type == "Int")
				{
					parameter.Value = param["Value"].as<int>();
				}
				else if (type == "Bool")
				{
					parameter.Value = param["Value"].as<bool>();
				}
				else if (type == "Vector2")
				{
					parameter.Value = param["Value"].as<glm::vec2>();
				}
				else if (type == "Vector3")
				{
					parameter.Value = param["Value"].as<glm::vec3>();
				}
				else if (type == "Vector4")
				{
					parameter.Value = param["Value"].as<glm::vec4>();
				}
				else if (type == "Matrix4")
				{
					glm::mat4 matrix(1.0f);
					auto matrixNode = param["Value"];
					for (int i = 0; i < 4; i++)
					{
						for (int j = 0; j < 4; j++)
						{
							matrix[i][j] = matrixNode[i][j].as<float>();
						}
					}
					parameter.Value = matrix;
				}
				else if (type == "AssetReference")
				{
					parameter.Value = UUID(param["Value"].as<uint64_t>());
				}

				parameters.push_back(parameter);
			}
		}

		return { parameters, pipelineUUID };
	}
}