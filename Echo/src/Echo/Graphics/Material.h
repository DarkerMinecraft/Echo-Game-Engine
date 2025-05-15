#pragma once

#include "AssetManager/Asset.h"
#include "Pipeline.h"

#include <variant>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Echo
{
	// Parameter types supported by materials
	using MaterialParamValue = std::variant<
		float,
		int,
		bool,
		glm::vec2,
		glm::vec3,
		glm::vec4,
		glm::mat4,
		UUID 
	>;

	struct MaterialParameter
	{
		std::string Name;
		MaterialParamValue Value;
		std::string ShaderParameterName; 
	};

	class Material
	{
	public:
		virtual ~Material() = default;

		virtual void Reload() = 0;
		virtual void Load() = 0;
		virtual void Destroy() = 0;

		virtual void SetParam(const std::string& name, const MaterialParamValue& value) = 0;
		virtual const MaterialParamValue& GetParam(const std::string& name) const = 0;
		virtual bool HasParam(const std::string& name) const = 0;

		static Ref<Material> Create(Ref<Pipeline> pipeline);
	};
}