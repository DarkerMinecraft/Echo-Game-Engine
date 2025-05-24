#pragma once

#include "Core/Base.h"

#include "AssetMetadata.h"
#include "Asset.h"

namespace Echo 
{

	class AssetRegistry
	{
	public:
		static void SetGlobalPath(const std::filesystem::path& globalPath);

		template<typename T>
		static Ref<T> LoadAsset(const std::filesystem::path& path);
		static void UnloadAsset(const std::filesystem::path& path);

		static void UnloadAllAssets();

		static std::vector<Ref<Asset>> GetAllLoadedAssets();

		static std::filesystem::path GetGlobalPath() { return s_GlobalPath; }
	private:
		static AssetType GetAssetTypeFromExtension(const std::string& extension);

		static Ref<Asset> CreateAsset(const AssetMetadata& metadata);
	private:
		static std::unordered_map<UUID, AssetMetadata> s_AssetMetadataMap;
		static std::unordered_map<std::filesystem::path, UUID> s_PathToUUID;
		static std::unordered_map<UUID, Ref<Asset>> s_LoadedAssets;

		static std::filesystem::path s_GlobalPath;
	};
}