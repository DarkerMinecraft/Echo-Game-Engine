#pragma once

#include "Asset/Asset.h"
#include "RHISpecification.h"
#include "Shader.h"

namespace Echo
{
	enum class PipelineType
	{
		Graphics,
		Compute
	};

	class Pipeline : public Asset
	{
	public:
		Pipeline(const std::string& name, PipelineType type);
		virtual ~Pipeline() = default;

		virtual void Reload() override;
		virtual void Unload() override;
		virtual void Destroy() override;

		virtual void Bind(CommandBuffer* cmd) = 0;

		PipelineType GetPipelineType() const { return m_Type; }
		const PipelineSpecification& GetSpecification() const { return m_Specification; }
		Ref<Shader> GetShader() const { return m_Shader; }

		void SetShader(Ref<Shader> shader) { m_Shader = shader; }

		// Resource binding
		virtual void BindResource(uint32_t binding, Ref<Texture2D> texture) = 0;
		virtual void BindResource(uint32_t binding, Ref<UniformBuffer> buffer) = 0;
		virtual void BindResource(uint32_t binding, Ref<Framebuffer> framebuffer, uint32_t attachmentIndex) = 0;

		// Factory methods
		static Ref<Pipeline> Create(Ref<Shader> shader, const PipelineSpecification& spec, PipelineType type);
		static Ref<Pipeline> Create(const std::filesystem::path& yamlPath);

		// YAML serialization/deserialization
		bool SerializeToYAML(const std::filesystem::path& filepath);
		static std::pair<PipelineSpecification, UUID> DeserializeFromYAML(const std::filesystem::path& filepath);
	protected:
		Ref<Shader> m_Shader;
		PipelineSpecification m_Specification;
		PipelineType m_Type;
	};
}