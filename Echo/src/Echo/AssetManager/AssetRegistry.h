#pragma once

#include "Core/Base.h"

#include "AssetMetadata.h"
#include "Asset.h"

namespace Echo 
{

	class AssetRegistry
	{
	public:
		AssetRegistry(const std::filesystem::path& globalPath);

		template<typename T>
		Ref<T> LoadAsset(const std::filesystem::path& path);
		void UnloadAsset(const std::filesystem::path& path);

		const std::filesystem::path GetGlobalPath() const { return m_GlobalPath; }
		std::filesystem::path GetGlobalPath() { return m_GlobalPath; }
	private:
		AssetType GetAssetTypeFromExtension(const std::string& extension);

		Ref<Asset> CreateAsset(const AssetMetadata& metadata);
	private:
		std::unordered_map<UUID, AssetMetadata> m_AssetMetadataMap;
		std::unordered_map<std::filesystem::path, UUID> m_PathToUUID;
		std::unordered_map<UUID, Ref<Asset>> m_LoadedAssets;

		std::filesystem::path m_GlobalPath;
	};
}