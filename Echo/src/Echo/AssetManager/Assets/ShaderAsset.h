#pragma once

#include "AssetManager/Asset.h"
#include "Graphics/Shader.h"
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

		virtual bool IsDirty() override { return m_Dirty; };
		virtual bool IsLoaded() override { return m_Loaded; };

		virtual AssetMetadata GetMetadata() override { return m_Metadata; }
		virtual const AssetMetadata GetMetadata() const { return m_Metadata; }

		Ref<Shader> GetShader() { return m_Shader; }
		bool CheckForChanges();
	private:
		AssetMetadata m_Metadata;
		Ref<ShaderCache> m_ShaderCache;

		Ref<Shader> m_Shader;
		const std::filesystem::path& path = "";
		bool m_Dirty = true, m_Loaded = false;

		float m_TimeSinceLastCheck = 0.0f;

		bool CompileShader();
		bool LoadFromCache();
		void SaveToCache();
		std::filesystem::path GetCachePath() const { return m_Metadata.CustomProps["Cache"]; };
	};

}