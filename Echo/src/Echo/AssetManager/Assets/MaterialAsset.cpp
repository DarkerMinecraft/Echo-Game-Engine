#include "pch.h"
#include "MaterialAsset.h"

namespace Echo
{

	MaterialAsset::~MaterialAsset()
	{

	}

	void MaterialAsset::Load()
	{
		
		m_Loaded = true;
	}

	void MaterialAsset::Reload()
	{

	}

	void MaterialAsset::Destroy()
	{
		if (!m_Loaded) return;

		m_Material->Destroy();
		m_Loaded = false;
	}

	bool MaterialAsset::CheckForChanges()
	{
		return false;
	}

	void MaterialAsset::AddShaderReflections(ShaderReflection& reflections)
	{

	}

}