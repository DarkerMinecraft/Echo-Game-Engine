#pragma once

#include "Graphics/CommandList.h"
#include "Graphics/Camera.h"
#include "Graphics/EditorCamera.h"

#include "Graphics/Mesh.h"
#include "Graphics/Material.h"

namespace Echo 
{

	class Renderer3D 
	{
	public:
		static void BeginScene(CommandList& cmd, const Camera& camera, const glm::mat4& transform);
		static void BeginScene(CommandList& cmd, const EditorCamera& camera);
		static void EndScene();

		static void SubmitMesh(Ref<Mesh> mesh, Ref<Material> material, const glm::mat4& transform);
		static void Shutdown();
	};

}