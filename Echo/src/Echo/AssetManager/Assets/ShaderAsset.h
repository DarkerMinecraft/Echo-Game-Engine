#pragma once

#include "AssetManager/Asset.h"

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

		virtual bool IsDirty() override { return m_Dirty; };
		virtual bool IsLoaded() override { return m_Loaded; };

		virtual AssetMetadata GetMetadata() override { return m_Metadata; };
		virtual const AssetMetadata GetMetadata() const { return m_Metadata; };
	private:
		AssetMetadata m_Metadata;

		bool m_Dirty = true, m_Loaded = false;
	};

}