#include "pch.h"
#include "AssetRegistry.h"

namespace Echo 
{

	AssetRegistry::AssetRegistry(const std::filesystem::path globalPath)
		: m_GlobalPath(globalPath)
	{
		if (!std::filesystem::exists(globalPath)) 
		{
			std::filesystem::create_directory(globalPath);
		}
	}

	void AssetRegistry::LoadAsset(const std::filesystem::path path)
	{

	}

	void AssetRegistry::UnloadAsset(const std::filesystem::path path)
	{

	}

}