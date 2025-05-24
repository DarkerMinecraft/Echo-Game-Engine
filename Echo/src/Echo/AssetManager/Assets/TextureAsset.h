#pragma once

#include "AssetManager/Asset.h"

#include "Graphics/Texture.h"

namespace Echo 
{

	class TextureAsset : public Asset 
	{
	public:
		TextureAsset(const AssetMetadata& metadata)
			: m_Metadata(metadata)
		{}
		virtual ~TextureAsset();

		virtual void Load() override;
		virtual void Reload() override;
		virtual void Destroy() override;

		virtual bool IsLoaded() override { return m_Loaded; }
		
		virtual bool CheckForChanges() override;

		virtual AssetMetadata GetMetadata() override { return m_Metadata; }
		virtual const AssetMetadata GetMetadata() const { return m_Metadata; }

		void* GetImGuiResourceID() { return m_Texture->GetImGuiResourceID(); }

		Ref<Texture2D> GetTexture() { return m_Texture; };
	private:
		AssetMetadata m_Metadata;
		bool m_Loaded = false;

		Ref<Texture2D> m_Texture;
	};

}