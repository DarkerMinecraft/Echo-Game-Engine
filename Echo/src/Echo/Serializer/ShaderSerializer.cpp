#include "pch.h"

#include "Graphics/Shader.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Echo
{
	bool Shader::SerializeToYAML(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		// Asset metadata
		out << YAML::Key << "Asset" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << "Shader";
		out << YAML::Key << "UUID" << YAML::Value << m_UUID;
		out << YAML::Key << "Name" << YAML::Value << m_Name;
		out << YAML::EndMap;

		// Shader specification
		out << YAML::Key << "ShaderSpecification" << YAML::Value << YAML::BeginMap;

		if (m_Specification.ShaderName)
			out << YAML::Key << "ShaderName" << YAML::Value << m_Specification.ShaderName;

		if (m_IsCompute)
		{
			if (m_Specification.ComputeShaderPath)
				out << YAML::Key << "ComputeShaderPath" << YAML::Value << m_Specification.ComputeShaderPath;

			if (m_Specification.ComputeShaderSource)
			{
				out << YAML::Key << "ComputeShaderSource" << YAML::Value << YAML::Literal
					<< m_Specification.ComputeShaderSource;
			}
		}
		else
		{
			if (m_Specification.VertexShaderPath)
				out << YAML::Key << "VertexShaderPath" << YAML::Value << m_Specification.VertexShaderPath;

			if (m_Specification.FragmentShaderPath)
				out << YAML::Key << "FragmentShaderPath" << YAML::Value << m_Specification.FragmentShaderPath;

			if (m_Specification.GeometryShaderPath)
				out << YAML::Key << "GeometryShaderPath" << YAML::Value << m_Specification.GeometryShaderPath;

			if (m_Specification.VertexShaderSource)
			{
				out << YAML::Key << "VertexShaderSource" << YAML::Value << YAML::Literal
					<< m_Specification.VertexShaderSource;
			}

			if (m_Specification.FragmentShaderSource)
			{
				out << YAML::Key << "FragmentShaderSource" << YAML::Value << YAML::Literal
					<< m_Specification.FragmentShaderSource;
			}

			if (m_Specification.GeometryShaderSource)
			{
				out << YAML::Key << "GeometryShaderSource" << YAML::Value << YAML::Literal
					<< m_Specification.GeometryShaderSource;
			}
		}

		out << YAML::EndMap;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	ShaderSpecification Shader::DeserializeFromYAML(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath);
		if (!stream.is_open())
		{
			EC_CORE_ERROR("Could not open shader file: {0}", filepath.string());
			return {};
		}

		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["ShaderSpecification"])
		{
			EC_CORE_ERROR("Invalid shader file: {0}", filepath.string());
			return {};
		}

		ShaderSpecification spec;
		auto shaderSpec = data["ShaderSpecification"];

		// Allocate strings on the heap since ShaderSpecification expects char*
		// These will be managed by the Shader class when it's created

		if (shaderSpec["ShaderName"])
			spec.ShaderName = strdup(shaderSpec["ShaderName"].as<std::string>().c_str());

		if (shaderSpec["VertexShaderPath"])
			spec.VertexShaderPath = strdup(shaderSpec["VertexShaderPath"].as<std::string>().c_str());

		if (shaderSpec["FragmentShaderPath"])
			spec.FragmentShaderPath = strdup(shaderSpec["FragmentShaderPath"].as<std::string>().c_str());

		if (shaderSpec["GeometryShaderPath"])
			spec.GeometryShaderPath = strdup(shaderSpec["GeometryShaderPath"].as<std::string>().c_str());

		if (shaderSpec["ComputeShaderPath"])
			spec.ComputeShaderPath = strdup(shaderSpec["ComputeShaderPath"].as<std::string>().c_str());

		if (shaderSpec["VertexShaderSource"])
			spec.VertexShaderSource = strdup(shaderSpec["VertexShaderSource"].as<std::string>().c_str());

		if (shaderSpec["FragmentShaderSource"])
			spec.FragmentShaderSource = strdup(shaderSpec["FragmentShaderSource"].as<std::string>().c_str());

		if (shaderSpec["GeometryShaderSource"])
			spec.GeometryShaderSource = strdup(shaderSpec["GeometryShaderSource"].as<std::string>().c_str());

		if (shaderSpec["ComputeShaderSource"])
			spec.ComputeShaderSource = strdup(shaderSpec["ComputeShaderSource"].as<std::string>().c_str());

		return spec;
	}
}