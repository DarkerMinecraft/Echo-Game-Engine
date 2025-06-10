#pragma once

#include "AssetManager/Asset.h"
#include "Graphics/Primitives/Mesh.h"

namespace Echo 
{

	class MeshAsset : public Asset
	{
	public:
		MeshAsset(const AssetMetadata& metadata)
			: m_Metadata(metadata)
		{}
		virtual ~MeshAsset();

		virtual void Load() override;
		virtual void Reload() override;
		virtual void Destroy() override;

		virtual bool IsLoaded() override { return m_Loaded; };

		virtual bool CheckForChanges() override;

		virtual AssetMetadata GetMetadata() override { return m_Metadata; }
		virtual const AssetMetadata GetMetadata() const { return m_Metadata; }

		Ref<Mesh> GetMesh() { return m_Mesh; }
	private:
		AssetMetadata m_Metadata;
		Ref<Mesh> m_Mesh;
	
		bool m_Loaded = false;

		float m_TimeSinceLastCheck = 0.0f;
	};

}