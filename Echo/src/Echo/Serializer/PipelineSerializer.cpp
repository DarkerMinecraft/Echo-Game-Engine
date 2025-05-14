#include "pch.h"
#include "Pipeline.h"
#include "Asset/AssetManager.h"

#include <yaml-cpp/yaml.h>
#include <fstream>

namespace Echo
{
	bool Pipeline::SerializeToYAML(const std::filesystem::path& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;

		// Asset metadata
		out << YAML::Key << "Asset" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << "Pipeline";
		out << YAML::Key << "UUID" << YAML::Value << (uint64_t)m_UUID;
		out << YAML::Key << "Name" << YAML::Value << m_Name;
		out << YAML::EndMap;

		// Pipeline data
		out << YAML::Key << "Pipeline" << YAML::Value << YAML::BeginMap;
		out << YAML::Key << "Type" << YAML::Value << (m_Type == PipelineType::Graphics ? "Graphics" : "Compute");

		// Shader reference
		if (m_Shader)
		{
			out << YAML::Key << "Shader" << YAML::Value << (uint64_t)m_Shader->GetUUID();
		}

		// Pipeline specification
		out << YAML::Key << "Specification" << YAML::Value << YAML::BeginMap;

		// Graphics pipeline specific
		if (m_Type == PipelineType::Graphics)
		{
			out << YAML::Key << "EnableBlending" << YAML::Value << m_Specification.EnableBlending;
			out << YAML::Key << "EnableDepthTest" << YAML::Value << m_Specification.EnableDepthTest;
			out << YAML::Key << "EnableDepthWrite" << YAML::Value << m_Specification.EnableDepthWrite;
			out << YAML::Key << "EnableCulling" << YAML::Value << m_Specification.EnableCulling;

			// Cull mode
			out << YAML::Key << "CullMode" << YAML::Value;
			switch (m_Specification.CullMode)
			{
				case Cull::None: out << "None"; break;
				case Cull::Front: out << "Front"; break;
				case Cull::Back: out << "Back"; break;
			}

			// Fill mode
			out << YAML::Key << "FillMode" << YAML::Value;
			switch (m_Specification.FillMode)
			{
				case Fill::Solid: out << "Solid"; break;
				case Fill::Wireframe: out << "Wireframe"; break;
			}

			// Depth compare op
			out << YAML::Key << "DepthCompareOp" << YAML::Value;
			switch (m_Specification.DepthCompareOp)
			{
				case CompareOp::Never: out << "Never"; break;
				case CompareOp::Less: out << "Less"; break;
				case CompareOp::Equal: out << "Equal"; break;
				case CompareOp::LessOrEqual: out << "LessOrEqual"; break;
				case CompareOp::Greater: out << "Greater"; break;
				case CompareOp::NotEqual: out << "NotEqual"; break;
				case CompareOp::GreaterOrEqual: out << "GreaterOrEqual"; break;
				case CompareOp::Always: out << "Always"; break;
			}

			// Topology
			out << YAML::Key << "Topology" << YAML::Value;
			switch (m_Specification.GraphicsTopology)
			{
				case Topology::TriangleList: out << "TriangleList"; break;
				case Topology::TriangleStrip: out << "TriangleStrip"; break;
				case Topology::LineList: out << "LineList"; break;
				case Topology::LineStrip: out << "LineStrip"; break;
				case Topology::PointList: out << "PointList"; break;
			}

			// Vertex layout
			out << YAML::Key << "VertexLayout" << YAML::Value << YAML::BeginSeq;
			for (const auto& element : m_Specification.VertexLayout)
			{
				out << YAML::BeginMap;
				out << YAML::Key << "Name" << YAML::Value << element.Name;

				out << YAML::Key << "Type" << YAML::Value;
				switch (element.Type)
				{
					case ShaderDataType::Float: out << "Float"; break;
					case ShaderDataType::Float2: out << "Float2"; break;
					case ShaderDataType::Float3: out << "Float3"; break;
					case ShaderDataType::Float4: out << "Float4"; break;
					case ShaderDataType::Int: out << "Int"; break;
					case ShaderDataType::Int2: out << "Int2"; break;
					case ShaderDataType::Int3: out << "Int3"; break;
					case ShaderDataType::Int4: out << "Int4"; break;
					case ShaderDataType::Mat3: out << "Mat3"; break;
					case ShaderDataType::Mat4: out << "Mat4"; break;
					case ShaderDataType::Bool: out << "Bool"; break;
					default: out << "None"; break;
				}

				out << YAML::Key << "Normalized" << YAML::Value << element.Normalized;
				out << YAML::EndMap;
			}
			out << YAML::EndSeq;
		}

		// Descriptor set layouts
		out << YAML::Key << "DescriptorSetLayouts" << YAML::Value << YAML::BeginSeq;
		for (const auto& layout : m_Specification.DescriptionSetLayouts)
		{
			out << YAML::BeginMap;
			out << YAML::Key << "Binding" << YAML::Value << layout.Binding;
			out << YAML::Key << "Count" << YAML::Value << layout.Count;

			out << YAML::Key << "Type" << YAML::Value;
			switch (layout.Type)
			{
				case DescriptorType::UniformBuffer: out << "UniformBuffer"; break;
				case DescriptorType::StorageBuffer: out << "StorageBuffer"; break;
				case DescriptorType::SampledImage: out << "SampledImage"; break;
				case DescriptorType::StorageImage: out << "StorageImage"; break;
			}

			out << YAML::Key << "Stage" << YAML::Value;
			switch (layout.Stage)
			{
				case ShaderStage::Vertex: out << "Vertex"; break;
				case ShaderStage::Fragment: out << "Fragment"; break;
				case ShaderStage::Compute: out << "Compute"; break;
				case ShaderStage::All: out << "All"; break;
			}

			out << YAML::EndMap;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap; // Specification
		out << YAML::EndMap; // Pipeline
		out << YAML::EndMap; // Root

		std::ofstream fout(filepath);
		fout << out.c_str();

		return true;
	}

	std::pair<PipelineSpecification, UUID> Pipeline::DeserializeFromYAML(const std::filesystem::path& filepath)
	{
		std::ifstream stream(filepath);
		if (!stream.is_open())
		{
			EC_CORE_ERROR("Could not open pipeline file: {0}", filepath.string());
			return { {}, UUID() };
		}

		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());
		if (!data["Pipeline"])
		{
			EC_CORE_ERROR("Invalid pipeline file: {0}", filepath.string());
			return { {}, UUID() };
		}

		PipelineSpecification spec;
		UUID shaderUUID;

		auto pipeline = data["Pipeline"];
		if (pipeline["Shader"])
		{
			shaderUUID = pipeline["Shader"].as<uint64_t>();
		}

		auto pipelineSpec = pipeline["Specification"];
		if (pipelineSpec)
		{
			// Parse specification properties
			if (pipelineSpec["EnableBlending"])
				spec.EnableBlending = pipelineSpec["EnableBlending"].as<bool>();

			if (pipelineSpec["EnableDepthTest"])
				spec.EnableDepthTest = pipelineSpec["EnableDepthTest"].as<bool>();

			if (pipelineSpec["EnableDepthWrite"])
				spec.EnableDepthWrite = pipelineSpec["EnableDepthWrite"].as<bool>();

			if (pipelineSpec["EnableCulling"])
				spec.EnableCulling = pipelineSpec["EnableCulling"].as<bool>();

			// Parse cull mode
			if (pipelineSpec["CullMode"])
			{
				std::string cullMode = pipelineSpec["CullMode"].as<std::string>();
				if (cullMode == "None")
					spec.CullMode = Cull::None;
				else if (cullMode == "Front")
					spec.CullMode = Cull::Front;
				else if (cullMode == "Back")
					spec.CullMode = Cull::Back;
			}

			// Parse fill mode
			if (pipelineSpec["FillMode"])
			{
				std::string fillMode = pipelineSpec["FillMode"].as<std::string>();
				if (fillMode == "Solid")
					spec.FillMode = Fill::Solid;
				else if (fillMode == "Wireframe")
					spec.FillMode = Fill::Wireframe;
			}

			// Parse depth compare op
			if (pipelineSpec["DepthCompareOp"])
			{
				std::string compareOp = pipelineSpec["DepthCompareOp"].as<std::string>();
				if (compareOp == "Never")
					spec.DepthCompareOp = CompareOp::Never;
				else if (compareOp == "Less")
					spec.DepthCompareOp = CompareOp::Less;
				else if (compareOp == "Equal")
					spec.DepthCompareOp = CompareOp::Equal;
				else if (compareOp == "LessOrEqual")
					spec.DepthCompareOp = CompareOp::LessOrEqual;
				else if (compareOp == "Greater")
					spec.DepthCompareOp = CompareOp::Greater;
				else if (compareOp == "NotEqual")
					spec.DepthCompareOp = CompareOp::NotEqual;
				else if (compareOp == "GreaterOrEqual")
					spec.DepthCompareOp = CompareOp::GreaterOrEqual;
				else if (compareOp == "Always")
					spec.DepthCompareOp = CompareOp::Always;
			}

			// Parse topology
			if (pipelineSpec["Topology"])
			{
				std::string topology = pipelineSpec["Topology"].as<std::string>();
				if (topology == "TriangleList")
					spec.GraphicsTopology = Topology::TriangleList;
				else if (topology == "TriangleStrip")
					spec.GraphicsTopology = Topology::TriangleStrip;
				else if (topology == "LineList")
					spec.GraphicsTopology = Topology::LineList;
				else if (topology == "LineStrip")
					spec.GraphicsTopology = Topology::LineStrip;
				else if (topology == "PointList")
					spec.GraphicsTopology = Topology::PointList;
			}

			// Parse vertex layout
			if (pipelineSpec["VertexLayout"])
			{
				BufferLayout layout;
				std::vector<BufferElement> elements;

				for (const auto& element : pipelineSpec["VertexLayout"])
				{
					std::string name = element["Name"].as<std::string>();
					bool normalized = element["Normalized"].as<bool>();

					// Parse shader data type
					std::string typeStr = element["Type"].as<std::string>();
					ShaderDataType type = ShaderDataType::None;

					if (typeStr == "Float") type = ShaderDataType::Float;
					else if (typeStr == "Float2") type = ShaderDataType::Float2;
					else if (typeStr == "Float3") type = ShaderDataType::Float3;
					else if (typeStr == "Float4") type = ShaderDataType::Float4;
					else if (typeStr == "Int") type = ShaderDataType::Int;
					else if (typeStr == "Int2") type = ShaderDataType::Int2;
					else if (typeStr == "Int3") type = ShaderDataType::Int3;
					else if (typeStr == "Int4") type = ShaderDataType::Int4;
					else if (typeStr == "Mat3") type = ShaderDataType::Mat3;
					else if (typeStr == "Mat4") type = ShaderDataType::Mat4;
					else if (typeStr == "Bool") type = ShaderDataType::Bool;

					elements.push_back({ type, name, normalized });
				}

				spec.VertexLayout = BufferLayout(elements);
			}

			// Parse descriptor set layouts
			if (pipelineSpec["DescriptorSetLayouts"])
			{
				for (const auto& layout : pipelineSpec["DescriptorSetLayouts"])
				{
					PipelineSpecification::DescriptionSetLayout descriptorLayout;

					descriptorLayout.Binding = layout["Binding"].as<uint32_t>();
					descriptorLayout.Count = layout["Count"].as<uint32_t>();

					// Parse descriptor type
					std::string typeStr = layout["Type"].as<std::string>();
					if (typeStr == "UniformBuffer")
						descriptorLayout.Type = DescriptorType::UniformBuffer;
					else if (typeStr == "StorageBuffer")
						descriptorLayout.Type = DescriptorType::StorageBuffer;
					else if (typeStr == "SampledImage")
						descriptorLayout.Type = DescriptorType::SampledImage;
					else if (typeStr == "StorageImage")
						descriptorLayout.Type = DescriptorType::StorageImage;

					// Parse shader stage
					std::string stageStr = layout["Stage"].as<std::string>();
					if (stageStr == "Vertex")
						descriptorLayout.Stage = ShaderStage::Vertex;
					else if (stageStr == "Fragment")
						descriptorLayout.Stage = ShaderStage::Fragment;
					else if (stageStr == "Compute")
						descriptorLayout.Stage = ShaderStage::Compute;
					else if (stageStr == "All")
						descriptorLayout.Stage = ShaderStage::All;

					spec.DescriptionSetLayouts.push_back(descriptorLayout);
				}
			}
		}

		return { spec, shaderUUID };
	}
}