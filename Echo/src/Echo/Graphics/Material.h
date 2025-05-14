// Echo/src/Echo/Graphics/Material.h
#pragma once

#include "Asset/Asset.h"
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
		UUID // Reference to texture or other assets
	>;

	struct MaterialParameter
	{
		std::string Name;
		MaterialParamValue Value;
		std::string ShaderParameterName; // For mapping to shader parameters
	};

	class Material : public Asset
	{
	public:
		Material(const std::string& name);
		virtual ~Material() = default;

		virtual void Reload() override;
		virtual void Unload() override;
		virtual void Destroy() override;

		virtual void Bind(CommandBuffer* cmd);

		// Parameter management
		void SetParam(const std::string& name, const MaterialParamValue& value);
		const MaterialParamValue& GetParam(const std::string& name) const;
		bool HasParam(const std::string& name) const;

		// References to dependent assets
		Ref<Pipeline> GetPipeline() const { return m_Pipeline; }
		void SetPipeline(Ref<Pipeline> pipeline) { m_Pipeline = pipeline; }

		// Factory methods
		static Ref<Material> Create(Ref<Pipeline> pipeline);
		static Ref<Material> Create(const std::filesystem::path& yamlPath);

		// YAML serialization/deserialization
		bool SerializeToYAML(const std::filesystem::path& filepath);
		static std::pair<std::vector<MaterialParameter>, UUID> DeserializeFromYAML(const std::filesystem::path& filepath);

	protected:
		Ref<Pipeline> m_Pipeline;
		std::unordered_map<std::string, MaterialParamValue> m_Parameters;
		std::unordered_map<std::string, uint32_t> m_ParameterBindings;

		void UpdateBindings();
	};
}