#pragma once

#include "AssetManager/Asset.h"
#include "Graphics/Shader.h"
#include "Graphics/Pipeline.h"

#include "Serializer/Cache/ShaderCache.h"

namespace Echo 
{

	class ShaderAsset : public Asset 
	{
	public:
		ShaderAsset(const AssetMetadata& metadata)
			: m_Metadata(metadata) {}
		virtual ~ShaderAsset();

		virtual void Load() override;
		virtual void Reload() override;
		virtual void Destroy() override;

		virtual bool IsLoaded() override { return m_Loaded; };

		virtual bool CheckForChanges() override;

		virtual AssetMetadata GetMetadata() override { return m_Metadata; }
		virtual const AssetMetadata GetMetadata() const { return m_Metadata; }

		Ref<Shader> GetShader() { return m_Shader; }

		Ref<Pipeline> GetPipeline() { return m_Pipeline; }
		void SetPipeline(Ref<Pipeline> pipeline) { m_Pipeline = pipeline; }
	private:
		AssetMetadata m_Metadata;
		Ref<ShaderCache> m_ShaderCache;

		Ref<Pipeline> m_Pipeline;

		Ref<Shader> m_Shader;
		bool m_Loaded = false;

		float m_TimeSinceLastCheck = 0.0f;
	};

}