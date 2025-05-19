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
		std::filesystem::path cachePath = GetCachePath();
		bool cacheExists = std::filesystem::exists(cachePath);

		if (cacheExists && LoadFromCache()) 
		{
			EC_CORE_INFO("Shader loaded from cache: {0}", m_Metadata.Path.string());

			m_Loaded = true;
			m_Dirty = false;
			return;
		}
		
		if (CompileShader())
		{
			SaveToCache();
			m_Loaded = true;
			m_Dirty = false;
		}
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

	}

	bool ShaderAsset::CompileShader()
	{

	}

	bool ShaderAsset::LoadFromCache()
	{
		m_ShaderCache = CreateRef<ShaderCache>(m_Metadata.ID);

		std::ofstream stream(GetCachePath(), std::ios::binary);
		if (!stream.is_open())
			return false;

		if (!m_ShaderCache->Deserialize(stream))
			return false;

		if (m_ShaderCache->IsOutdated())
			return false;
	}

	void ShaderAsset::SaveToCache()
	{

	}

}