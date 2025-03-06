#pragma once

#include <Echo/Core/Layer.h>
#include <Echo/Graphics/Image.h>
#include <Echo/Graphics/Pipeline.h>

#include <Echo/Graphics/Buffer.h>

namespace Echo
{

	class EditorLayer : public Layer
	{
	public:
		EditorLayer();
		virtual ~EditorLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		virtual void OnUpdate(Timestep ts) override;

		virtual void OnEvent(Event& e) override;

		virtual void OnImGuiRender() override;

		virtual void Destroy() override;
	private:
		Ref<Image> m_DrawImage;

		Ref<Pipeline> m_ComputePipeline;
		Ref<Pipeline> m_TrianglePipeline;

		Ref<VertexBuffer> m_TriangleVertexBuffer;
	};
}

