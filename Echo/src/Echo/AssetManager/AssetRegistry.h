#pragma once

#include "AssetMetadata.h"
#include "Asset.h"

namespace Echo 
{

	class AssetRegistry
	{
	public:
	private:
		std::unordered_map<UUID, AssetMetadata> m_AssetMetadataMap;
		std::unordered_map<std::filesystem::path, UUID> m_PathToUUID;
		std::unordered_map<UUID, Asset> m_LoadedAssets;
	};

}