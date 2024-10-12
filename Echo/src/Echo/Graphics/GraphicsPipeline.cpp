#include "pch.h"
#include "GraphicsPipeline.h"

#include "RendererAPI.h"
#include "Echo/Core/Application.h"
#include "Platform/Vulkan/VulkanPipeline.h"

namespace Echo
{
	
	Ref<GraphicsPipeline> GraphicsPipeline::Create(const std::string& name, GraphicsPipelineData& data)
	{
		switch(RendererAPI::GetAPI())
		{
			case RendererAPI::API::None: return nullptr;
			case RendererAPI::API::Vulkan: return CreateRef<VulkanPipeline>(name, (VulkanGraphicsContext*)Application::Get().GetWindow().GetContext(), data);
			case RendererAPI::API::DirectX12: return nullptr;
		}
	}

	void PipelineLibrary::Add(const Ref<GraphicsPipeline>& pipeline)
	{
		auto& name = pipeline->GetName();
		Add(name, pipeline);
	}

	void PipelineLibrary::Add(const std::string& name, const Ref<GraphicsPipeline> pipeline)
	{
		EC_CORE_ASSERT(m_Pipelines.find(name) == m_Pipelines.end(), "Pipeline already exists!")
		m_Pipelines[name] = pipeline;
	}

	Ref<GraphicsPipeline> PipelineLibrary::Load(const std::string& name, const std::string& vertexShaderPath, const std::string& fragmentShaderPath)
	{
		GraphicsPipelineData data{};
		data.VertexShaderPath = vertexShaderPath.c_str();
		data.FragmentShaderPath = fragmentShaderPath.c_str();

		auto pipeline = GraphicsPipeline::Create(name, data);
		Add(name, pipeline);
		return pipeline;
	}

	Ref<GraphicsPipeline> PipelineLibrary::Get(const std::string& name)
	{
		EC_CORE_ASSERT(m_Pipelines.find(name) != m_Pipelines.end(), "Pipeline not found!");
		return m_Pipelines[name];
	}

}
