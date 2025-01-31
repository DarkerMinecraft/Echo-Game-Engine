#include "MinecraftLayer.h"

#include <Echo/Graphics/RHI.h>

#include <imgui.h>

namespace Minecraft 
{


	MinecraftLayer::MinecraftLayer()
	{

	}

	void MinecraftLayer::OnAttach()
	{
		Echo::FrameBufferDesc backgroundFrameBufferDesc{};
		backgroundFrameBufferDesc.UseDrawImage = true;

		m_BackgroundFrameBuffer = Echo::RHI::CreateFrameBuffer(backgroundFrameBufferDesc);
		
		Echo::PipelineDesc backgroundComputeShader{};
		backgroundComputeShader.ComputeShaderPath = L"assets/shaders/nightsky.hlsl";
		
		backgroundComputeShader.DescriptorSetLayouts = 
		{
			{0, Echo::DescriptorType::UniformBuffer, 1, Echo::ShaderStage::Compute },
			{1, Echo::DescriptorType::StorageImage, 1, Echo::ShaderStage::Compute },
		};

		backgroundComputeShader.MaxSets = 2;

		//m_BackgroundComputePipeline = Echo::RHI::CreatePipeline(backgroundComputeShader);

		Echo::BufferDesc nightSkyBufferDesc{};
		
		m_Data1 = { 0, 0, 0, 0 };
		nightSkyBufferDesc.InitialData = &m_Data1;
		nightSkyBufferDesc.Usage = Echo::BufferUsage::BUFFER_USAGE_UNIFORM;
		nightSkyBufferDesc.Flags = Echo::MemoryFlags::MEMORY_HOST_VISIBLE | Echo::MemoryFlags::MEMORY_HOST_COHERENT;
		nightSkyBufferDesc.BufferSize = sizeof(glm::vec4);

		m_NightSkyBuffer = Echo::RHI::CreateBuffer(nightSkyBufferDesc);

		m_TriangleMesh = Echo::RHI::CreateMesh({
			{{-0.5f, -0.5f}, {-1.0f, -1.0f}},
			{{0.5f, -0.5f}, {1.0f, -1.0f}},
			{{0.5f, 0.5f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f}, {-1.0f, 1.0f}}
		}, 
			{ 0, 1, 2, 2, 3, 0 }
		);

	}

	void MinecraftLayer::OnDetach()
	{

	}

	void MinecraftLayer::OnUpdate(Echo::Timestep ts)
	{
		{
			m_TriangleMesh->Bind();
			Echo::RHI::CMDDrawIndexed(m_TriangleMesh->GetIndicesSize());
		}
	}

	void MinecraftLayer::OnEvent(Echo::Event& e)
	{

	}

	void MinecraftLayer::OnImGuiRender()
	{
		
	}

	void MinecraftLayer::Destroy()
	{

	}

}