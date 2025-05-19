#pragma once

#include "AssetManager/Asset.h"
#include "Graphics/Pipeline.h"

namespace Echo
{

	class PipelineAsset : public Asset
	{
	public:
		PipelineAsset(const AssetMetadata& metadata)
			: m_Metadata(metadata)
		{}
		virtual ~PipelineAsset();

		virtual void Load() override;
		virtual void Reload() override;
		virtual void Destroy() override;

		virtual bool IsDirty() override { return m_Dirty; };
		virtual bool IsLoaded() override { return m_Loaded; };

		virtual AssetMetadata GetMetadata() override { return m_Metadata; }
		virtual const AssetMetadata GetMetadata() const { return m_Metadata; }

		Ref<Pipeline> GetPipeline() { return m_Pipeline; }
	private:
		AssetMetadata m_Metadata;

		Ref<Pipeline> m_Pipeline;
		const std::filesystem::path& path = "";
		bool m_Dirty = true, m_Loaded = false;
	};

}