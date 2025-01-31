#pragma once

#include <Echo/Core/Layer.h>

#include <Echo/Graphics/FrameBuffer.h>
#include <Echo/Graphics/Pipeline.h>
#include <Echo/Graphics/Mesh.h>

#include <glm/glm.hpp>

namespace Minecraft 
{

	class MinecraftLayer : public Echo::Layer
	{
	public:
		MinecraftLayer();
		virtual ~MinecraftLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Echo::Timestep ts) override;

		virtual void OnEvent(Echo::Event& e) override;

		virtual void OnImGuiRender() override;

		virtual void Destroy() override;
	private:
		Echo::Ref<Echo::FrameBuffer> m_BackgroundFrameBuffer;
		Echo::Ref<Echo::Buffer> m_NightSkyBuffer;
		Echo::Ref<Echo::Pipeline> m_BackgroundComputePipeline;

		Echo::Ref<Echo::Mesh> m_TriangleMesh;

		glm::vec4 m_Data1;
	};

}