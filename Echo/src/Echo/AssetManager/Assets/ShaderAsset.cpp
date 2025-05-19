#include "pch.h"
#include "ShaderAsset.h"

#include <fstream>

namespace Echo 
{

	ShaderAsset::~ShaderAsset()
	{
		Destroy();
	}

	void ShaderAsset::Load()
	{
		m_Shader = Shader::Create(m_Metadata.Path);

		m_Loaded = true;
		m_Dirty = false;
	}

	void ShaderAsset::Reload()
	{

	}

	void ShaderAsset::Destroy()
	{
		m_Shader->Destroy();
	}

	bool ShaderAsset::CheckForChanges()
	{
		return false;
	}

}