#include "pch.h"
#include "AssetRegistry.h"

#include "Assets/ShaderAsset.h"
#include "Assets/TextureAsset.h"
#include "Assets/MeshAsset.h"
#include "Assets/MaterialAsset.h"

#include <fstream>

namespace Echo
{

	std::unordered_map<UUID, Echo::AssetMetadata> AssetRegistry::s_AssetMetadataMap;
	std::unordered_map<std::filesystem::path, UUID> AssetRegistry::s_PathToUUID;
	std::unordered_map<UUID, Ref<Echo::Asset>> AssetRegistry::s_LoadedAssets;
	std::filesystem::path AssetRegistry::s_GlobalPath;

	template<>
	Ref<ShaderAsset> AssetRegistry::LoadAsset<ShaderAsset>(const std::filesystem::path& path)
	{
		EC_PROFILE_FUNCTION();
		std::filesystem::path fullPath = s_GlobalPath / path;

		if (!std::filesystem::exists(fullPath))
		{
			EC_CORE_ERROR("Failed to load asset: file doesn't exist: {0}", fullPath.string());
			return nullptr;
		}

		if (s_PathToUUID.contains(fullPath))
		{
			UUID id = s_PathToUUID[fullPath];
			return Cast<ShaderAsset>(s_LoadedAssets[id]);
		}

		std::filesystem::path metaPath = fullPath.string() + ".meta";

		AssetMetadata metadata;
		if (std::filesystem::exists(metaPath))
		{
			metadata.DeserializeFromFile(metaPath);
		}
		else
		{
			metadata.Path = fullPath;
			metadata.Type = GetAssetTypeFromExtension(path.extension().string());
			metadata.LastModified = std::filesystem::last_write_time(fullPath);
			metadata.SerializeToFile(metaPath);
		}

		Ref<Asset> asset = CreateAsset(metadata);
		if (asset)
		{
			asset->Load();
			s_LoadedAssets[metadata.ID] = asset;
			s_PathToUUID[path] = metadata.ID;
			s_AssetMetadataMap[metadata.ID] = metadata;
			return Cast<ShaderAsset>(asset);
		}
	}

	template<>
	Ref<MaterialAsset> AssetRegistry::LoadAsset<MaterialAsset>(const std::filesystem::path& path)
	{
		EC_PROFILE_FUNCTION();
		std::filesystem::path fullPath = s_GlobalPath / path;

		if (!std::filesystem::exists(fullPath))
		{
			EC_CORE_ERROR("Failed to load asset: file doesn't exist: {0}", fullPath.string());
			return nullptr;
		}

		if (s_PathToUUID.contains(fullPath))
		{
			UUID id = s_PathToUUID[fullPath];
			return Cast<MaterialAsset>(s_LoadedAssets[id]);
		}

		std::filesystem::path metaPath = fullPath.string() + ".meta";

		AssetMetadata metadata;
		if (std::filesystem::exists(metaPath))
		{
			metadata.DeserializeFromFile(metaPath);
		}
		else
		{
			metadata.Path = fullPath;
			metadata.Type = GetAssetTypeFromExtension(path.extension().string());
			metadata.LastModified = std::filesystem::last_write_time(fullPath);
			metadata.SerializeToFile(metaPath);
		}

		Ref<Asset> asset = CreateAsset(metadata);
		if (asset)
		{
			asset->Load();
			s_LoadedAssets[metadata.ID] = asset;
			s_PathToUUID[path] = metadata.ID;
			s_AssetMetadataMap[metadata.ID] = metadata;
			return Cast<MaterialAsset>(asset);
		}
	}

	template<>
	Ref<MeshAsset> AssetRegistry::LoadAsset<MeshAsset>(const std::filesystem::path& path)
	{
		EC_PROFILE_FUNCTION();
		std::filesystem::path fullPath = s_GlobalPath / path;

		if (!std::filesystem::exists(fullPath))
		{
			EC_CORE_ERROR("Failed to load asset: file doesn't exist: {0}", fullPath.string());
			return nullptr;
		}

		if (s_PathToUUID.contains(fullPath))
		{
			UUID id = s_PathToUUID[fullPath];
			return Cast<MeshAsset>(s_LoadedAssets[id]);
		}

		std::filesystem::path metaPath = fullPath.string() + ".meta";
		AssetMetadata metadata;
		if (std::filesystem::exists(metaPath))
		{
			metadata.DeserializeFromFile(metaPath);
		}
		else
		{
			metadata.Path = fullPath;
			metadata.Type = GetAssetTypeFromExtension(path.extension().string());
			metadata.LastModified = std::filesystem::last_write_time(fullPath);
			metadata.SerializeToFile(metaPath);
		}

		Ref<Asset> asset = CreateAsset(metadata);
		if (asset)
		{
			asset->Load();
			s_LoadedAssets[metadata.ID] = asset;
			s_PathToUUID[path] = metadata.ID;
			s_AssetMetadataMap[metadata.ID] = metadata;
			return Cast<MeshAsset>(asset);
		}
	}

	template<>
	Ref<TextureAsset> AssetRegistry::LoadAsset<TextureAsset>(const std::filesystem::path& path)
	{
		EC_PROFILE_FUNCTION();

		if (path == "")
			return nullptr;

		std::filesystem::path fullPath = s_GlobalPath / path;

		if (!std::filesystem::exists(fullPath))
		{
			EC_CORE_ERROR("Failed to load asset: file doesn't exist: {0}", fullPath.string());
			return nullptr;
		}

		if (s_PathToUUID.contains(fullPath))
		{
			UUID id = s_PathToUUID[fullPath];
			return Cast<TextureAsset>(s_LoadedAssets[id]);
		}

		std::filesystem::path metaPath = fullPath.string() + ".meta";

		AssetMetadata metadata;
		if (std::filesystem::exists(metaPath))
		{
			metadata.DeserializeFromFile(metaPath);
		}
		else
		{
			metadata.Path = fullPath;
			metadata.Type = GetAssetTypeFromExtension(path.extension().string());
			metadata.LastModified = std::filesystem::last_write_time(fullPath);
			metadata.CustomProps["MinFilter"] = 1;
			metadata.CustomProps["MagFilter"] = 1;
			metadata.SerializeToFile(metaPath);
		}

		Ref<Asset> asset = CreateAsset(metadata);
		if (asset)
		{
			asset->Load();
			s_LoadedAssets[metadata.ID] = asset;
			s_PathToUUID[path] = metadata.ID;
			s_AssetMetadataMap[metadata.ID] = metadata;
			return Cast<TextureAsset>(asset);
		}
	}

	void AssetRegistry::SetGlobalPath(const std::filesystem::path& globalPath)
	{
		if (!std::filesystem::exists(globalPath))
		{
			std::filesystem::create_directory(globalPath);
		}

		s_GlobalPath = globalPath;
	}

	void AssetRegistry::UnloadAsset(const std::filesystem::path& path)
	{
		Ref<Asset> asset = s_LoadedAssets[s_PathToUUID[path]];
		asset->Destroy();
	}

	void AssetRegistry::UnloadAllAssets()
	{
		for (auto& [uuid, asset] : s_LoadedAssets) 
		{
			s_AssetMetadataMap[uuid].SerializeToFile(s_AssetMetadataMap[uuid].Path.string() + ".meta");
			asset->Destroy();
		}
		
		s_LoadedAssets.clear();
		s_PathToUUID.clear();
		s_AssetMetadataMap.clear();
	}

	std::vector<Ref<Asset>> AssetRegistry::GetAllLoadedAssets()
	{
		std::vector<Ref<Asset>> loadedAssets;
		for (auto& [uuid, _] : s_LoadedAssets)
		{
			loadedAssets.push_back(s_LoadedAssets[uuid]);
		}
		return loadedAssets;
	}

	void AssetRegistry::UpdateAssetMetadata(AssetMetadata& metadata)
	{
		s_AssetMetadataMap[metadata.ID] = metadata;
		std::filesystem::path metaPath = metadata.Path.string() + ".meta";
		metadata.SerializeToFile(metaPath);
	}

	AssetType AssetRegistry::GetAssetTypeFromExtension(const std::string& extension)
	{
		if (extension == ".slang" || extension == ".shader")
			return AssetType::Shader;
		else if (extension == ".mat" || extension == ".material")
			return AssetType::Material;
		else if (extension == ".png")
			return AssetType::Texture;
		else if (extension == ".obj")
			return AssetType::Mesh;
		else if (extension == ".echo")
			return AssetType::Scene;

		EC_CORE_WARN("Unknown asset extension: {0}", extension);
		return AssetType::None;
	}

	Ref<Asset> AssetRegistry::CreateAsset(const AssetMetadata& metadata)
	{
		switch (metadata.Type)
		{
			case AssetType::Shader:
				return CreateRef<ShaderAsset>(metadata);
			case AssetType::Texture:
				return CreateRef<TextureAsset>(metadata);
			case AssetType::Mesh:
				return CreateRef<MeshAsset>(metadata);
		}

		EC_CORE_ERROR("Unknown asset type for: {0}", metadata.Path.string());
		return nullptr;
	}

}