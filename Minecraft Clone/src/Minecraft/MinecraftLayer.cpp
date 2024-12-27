#include "MinecraftLayer.h"

#include <Echo/Graphics/RHI.h>

namespace Minecraft 
{

	struct NightSkyPushConstant
	{
		float Time;
		float Intensity;
	};

	MinecraftLayer::MinecraftLayer()
	{

	}

	void MinecraftLayer::OnAttach()
	{
		Echo::FrameBufferDesc backgroundFbDesc{};
		backgroundFbDesc.UseSwapchain = true;
		backgroundFbDesc.ClearOnBegin = true;

		m_BackgroundFrameBuffer = Echo::RHI::CreateFrameBuffer(backgroundFbDesc);

		Echo::PipelineDesc backgroundComputeShader{};
		backgroundComputeShader.ComputeShaderPath = "assets/shaders/nightsky.slang";
		backgroundComputeShader.PushConstants = 
		{
			0, 
			sizeof(NightSkyPushConstant)
		};

		m_BackgroundComputePipeline = Echo::RHI::CreatePipeline(backgroundComputeShader);
	}

	void MinecraftLayer::OnDetach()
	{

	}

	void MinecraftLayer::OnUpdate(Echo::Timestep ts)
	{
		{
			m_BackgroundFrameBuffer->Start();

			m_BackgroundComputePipeline->Bind();
			NightSkyPushConstant constants
			{
				3, 5
			};

			m_BackgroundComputePipeline->WritePushConstants(&constants);
			Echo::RHI::CMDDispatch(1.0 / 16.0, 1.0 / 16.0);

			m_BackgroundFrameBuffer->End();
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