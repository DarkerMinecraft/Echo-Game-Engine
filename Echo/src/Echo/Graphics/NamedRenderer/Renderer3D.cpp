#include "pch.h"
#include "Renderer3D.h"

#include "Graphics/Material.h"
#include "Graphics/Mesh.h"
#include "Graphics/CommandList.h"

#include "Graphics/Camera.h"
#include "Graphics/EditorCamera.h"

namespace Echo 
{

	struct Renderer3DData 
	{
		std::unordered_map<Ref<Material>, Ref<Mesh>> Meshes;

		CommandList* Cmd;
	};

	static Renderer3DData s_Data;

	void Renderer3D::BeginScene(CommandList& cmd, const Camera& camera, const glm::mat4& transform)
	{
		s_Data.Cmd = &cmd;
	}

	void Renderer3D::BeginScene(CommandList& cmd, const EditorCamera& camera)
	{
		s_Data.Cmd = &cmd;
	}

	void Renderer3D::EndScene()
	{
		for (auto& [material, mesh] : s_Data.Meshes)
		{
			mesh->GetVertexBuffer()->Bind(s_Data.Cmd);
			mesh->GetIndexBuffer()->Bind(s_Data.Cmd);
			
			s_Data.Cmd->BindPipeline(pipeline);
			material.SetShaderUniforms();
			
			s_Data.Cmd->DrawIndexed(mesh->GetIndexBuffer()->GetCount());
		}
	}

	void Renderer3D::SubmitMesh(Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform)
	{

	}

	void Renderer3D::Shutdown()
	{

	}

}