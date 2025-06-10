#pragma once

#include "AssetManager/Asset.h"
#include "Graphics/Primitives/Material.h"

#include "Reflections/ShaderReflection.h"

namespace Echo
{

	class MaterialAsset : public Asset
	{
	public:
		MaterialAsset(const AssetMetadata& metadata)
			: m_Metadata(metadata)
		{}
		virtual ~MaterialAsset();

		virtual void Load() override;
		virtual void Reload() override;
		virtual void Destroy() override;

		virtual bool IsLoaded() override { return m_Loaded; };

		virtual bool CheckForChanges() override;

		virtual AssetMetadata GetMetadata() override { return m_Metadata; }
		virtual const AssetMetadata GetMetadata() const { return m_Metadata; }

		Ref<Material> GetMaterial() { return m_Material; }

		void AddShaderReflections(ShaderReflection& reflections);
	private:
		AssetMetadata m_Metadata;
		Ref<Material> m_Material;

		bool m_Loaded = false;

		float m_TimeSinceLastCheck = 0.0f;
	};

}