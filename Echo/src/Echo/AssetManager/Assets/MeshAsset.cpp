#include "pch.h"
#include "MeshAsset.h"

namespace Echo 
{

	MeshAsset::~MeshAsset()
	{

	}

	void MeshAsset::Load()
	{
		m_Mesh = Mesh::Create(m_Metadata.Path.string());

		m_Loaded = true;
	}

	void MeshAsset::Reload()
	{

	}

	void MeshAsset::Destroy()
	{
		if (!m_Loaded) return;

		m_Mesh->Destroy();
		m_Loaded = false;
	}

	bool MeshAsset::CheckForChanges()
	{
		return false;
	}

}