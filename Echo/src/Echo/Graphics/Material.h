#pragma once 

#include "Echo/Core/Base.h"

#include "Shader.h"
#include "Texture.h"

#include <glm/glm.hpp>

namespace Echo 
{

	class Material 
	{
	public:
		virtual ~Material() = default;

		virtual void Destroy() = 0; 

		virtual Shader* GetShader() = 0;
		virtual Texture2D* GetTexture() = 0;

		static Ref<Material> Create(Ref<Shader> shader, Ref<Texture2D> texture);
		static Ref<Material> Create(Ref<Shader> shader, glm::vec3 color);
	};

}