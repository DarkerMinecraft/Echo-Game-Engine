#include "pch.h"
#include "VulkanModel.h"

#include "VulkanCommandList.h"

namespace Echo 
{

	VulkanModel::VulkanModel(const std::vector<Vertex> vertices)
	{
		CreateModel(vertices);
	}

	VulkanModel::~VulkanModel()
	{
		m_VertexBuffer.reset();
	}

	void VulkanModel::Bind()
	{
		m_VertexBuffer->Bind();
	}

	void VulkanModel::CreateModel(const std::vector<Vertex> vertices)
	{
		m_VertexBuffer = CreateRef<VulkanVertexBuffer>(vertices);
	}

}