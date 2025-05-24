#include "pch.h"
#include "ShaderAsset.h"

#include "AssetManager/AssetRegistry.h"
#include <fstream>

namespace Echo 
{

	ShaderAsset::~ShaderAsset()
	{
		Destroy();
	}

	void ShaderAsset::Load()
	{
		bool hasChanged = CheckForChanges();
		m_Shader = Shader::Create(m_Metadata.Path, hasChanged);

		m_Loaded = true;
	}

	void ShaderAsset::Reload()
	{
		Ref<Shader> tempShader;
		bool didCompile;
		tempShader = Shader::Create(m_Metadata.Path, true, &didCompile);

		if (didCompile)
		{
			m_Loaded = false;

			m_Shader->Destroy();
			m_Shader = tempShader;
			m_Pipeline->ReconstructPipeline(m_Shader);

			std::filesystem::file_time_type pathLastModified = std::filesystem::last_write_time(m_Metadata.Path);
			m_Metadata.LastModified = pathLastModified;
			AssetRegistry::UpdateAssetMetadata(m_Metadata);

			m_Loaded = true;
		}
		else
		{		
			if (tempShader)
			{
				tempShader->Destroy();
			}
		}
	}

	void ShaderAsset::Destroy()
	{
		if (!m_Loaded) return;

		m_Shader->Destroy();
		m_Loaded = false;
	}

	bool ShaderAsset::CheckForChanges()
	{
		std::filesystem::file_time_type pathLastModified = std::filesystem::last_write_time(m_Metadata.Path);
		std::filesystem::file_time_type metaLastModifed = m_Metadata.LastModified;

		auto pathDuration = pathLastModified.time_since_epoch();
		auto metaDuration = metaLastModifed.time_since_epoch();

		auto pathSeconds = std::chrono::duration_cast<std::chrono::seconds>(pathDuration);
		auto metaSeconds = std::chrono::duration_cast<std::chrono::seconds>(metaDuration);

		return metaSeconds != pathSeconds;
	}

}