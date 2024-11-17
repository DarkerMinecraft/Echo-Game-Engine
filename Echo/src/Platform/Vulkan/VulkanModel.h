#pragma once

#include "Echo/Graphics/Model.h"

#include "Echo/Graphics/CommandList.h"

namespace Echo 
{
	class VulkanModel : public Model 
	{
	public:
		VulkanModel(const std::vector<Vertex> vertices);
		~VulkanModel();

		virtual void Bind() override;
		virtual uint32_t GetVertexCount() override { return static_cast<uint32_t>(m_Vertices.size()); }
	private:
		void CreateModel(const std::vector<Vertex> vertices);
	private:
		Ref<VertexBuffer> m_VertexBuffer;

		const std::vector<Vertex> m_Vertices;
	};
}