#include "pch.h"
#include "Renderer3D.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/CommandList.h"

#include "Graphics/Camera.h"
#include "Graphics/EditorCamera.h"

namespace Echo
{

	struct CameraData
	{
		glm::mat4 ProjViewMatrix;
	};

	struct RenderQueue 
	{
		std::unordered_map<Ref<Material>, Ref<Mesh>> Model;
		glm::mat4 Transform;
	};

	struct Renderer3DData
	{
		std::vector<RenderQueue> Meshes;

		Ref<UniformBuffer> CameraBuffer;

		CommandList* Cmd;
	};

	static Renderer3DData s_Data;

	void Renderer3D::BeginScene(CommandList& cmd, const Camera& camera, const glm::mat4& transform)
	{
		s_Data.Cmd = &cmd;

		CameraData camData = 
		{
			.ProjViewMatrix = camera.GetProjection() * transform
		};
		s_Data.CameraBuffer = UniformBuffer::Create(&camData, sizeof(CameraData));
	}

	void Renderer3D::BeginScene(CommandList& cmd, const EditorCamera& camera)
	{
		s_Data.Cmd = &cmd;

		CameraData camData
		{
			.ProjViewMatrix = camera.GetProjection() * camera.GetViewMatrix()
		};
		s_Data.CameraBuffer = UniformBuffer::Create(&camData, sizeof(CameraData));
	}

	void Renderer3D::EndScene()
	{
		for (auto& queue : s_Data.Meshes)
		{
			Ref<UniformBuffer> modelTransformBuffer = UniformBuffer::Create(&queue.Transform, sizeof(glm::mat4));
			for (auto& [material, mesh] : queue.Model)
			{
				Pipeline* pipeline = material->GetPipeline();
				s_Data.Cmd->BindPipeline(pipeline);
				s_Data.Cmd->BindVertexBuffer(mesh->GetVertexBuffer());
				s_Data.Cmd->BindIndicesBuffer(mesh->GetIndexBuffer());
				pipeline->WriteDescriptorUniformBuffer(s_Data.CameraBuffer, 0);
				pipeline->WriteDescriptorUniformBuffer(modelTransformBuffer, 1);
				if (material->GetTexture() != nullptr)
				{
					pipeline->WriteDescriptorCombinedTexture(material->GetTexture(), 0);
				}

				s_Data.Cmd->DrawIndexed(mesh->GetIndexBuffer()->GetIndicesCount(), 0, 0, 0, 0);
			}
		}

		s_Data.Meshes.clear();
		s_Data.CameraBuffer.reset();
	}

	void Renderer3D::SubmitMesh(Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform)
	{
		s_Data.Meshes.push_back({ {{ material, mesh }}, transform });
	}

	void Renderer3D::Shutdown()
	{

	}

}