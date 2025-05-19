#include "pch.h"
#include "AssetRegistry.h"

#include "Assets/ShaderAsset.h"
#include "Assets/PipelineAsset.h"

#include <fstream>

namespace Echo
{

	AssetRegistry::AssetRegistry(const std::filesystem::path& globalPath)
		: m_GlobalPath(globalPath)
	{
		if (!std::filesystem::exists(globalPath))
		{
			std::filesystem::create_directory(globalPath);
		}
	}

	template<>
	Ref<ShaderAsset> AssetRegistry::LoadAsset<ShaderAsset>(const std::filesystem::path& path)
	{
		std::filesystem::path fullPath = m_GlobalPath / path;

		if (!std::filesystem::exists(fullPath))
		{
			EC_CORE_ERROR("Failed to load asset: file doesn't exist: {0}", fullPath.string());
			return nullptr;
		}

		if (m_PathToUUID.contains(fullPath))
		{
			UUID id = m_PathToUUID[fullPath];
			return Cast<ShaderAsset>(m_LoadedAssets[id]);
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
			metadata.CustomProps["Cache"] = (fullPath.string() + ".cache");
			metadata.SerializeToFile(metaPath);
		}

		Ref<Asset> asset = CreateAsset(metadata);
		if (asset)
		{
			asset->Load();
			m_LoadedAssets[metadata.ID] = asset;
			m_PathToUUID[path] = metadata.ID;
			m_AssetMetadataMap[metadata.ID] = metadata;
			return Cast<ShaderAsset>(asset);
		}
	}

	template<>
	Ref<PipelineAsset> AssetRegistry::LoadAsset<PipelineAsset>(const std::filesystem::path& path)
	{
		std::filesystem::path fullPath = m_GlobalPath / path;

		if (!std::filesystem::exists(fullPath))
		{
			EC_CORE_ERROR("Failed to load asset: file doesn't exist: {0}", fullPath.string());
			return nullptr;
		}

		if (m_PathToUUID.contains(fullPath))
		{
			UUID id = m_PathToUUID[fullPath];
			return Cast<PipelineAsset>(m_LoadedAssets[id]);
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
			metadata.CustomProps["EnableBlending"] = false;
			metadata.CustomProps["BlendFactor"] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			metadata.CustomProps["EnableDepthTest"] = false;
			metadata.CustomProps["EnableDepthWrite"] = false;
			metadata.CustomProps["EnableCulling"] = true;
			metadata.CustomProps["CullMode"] = 1;
			metadata.CustomProps["FillMode"] = 0;
			metadata.CustomProps["DepthCompareOp"] = 1;
			metadata.CustomProps["GraphicsTopology"] = 0;

			metadata.SerializeToFile(metaPath);
		}

		Ref<Asset> asset = CreateAsset(metadata);
		if (asset)
		{
			asset->Load();
			m_LoadedAssets[metadata.ID] = asset;
			m_PathToUUID[path] = metadata.ID;
			m_AssetMetadataMap[metadata.ID] = metadata;
			return Cast<PipelineAsset>(asset);
		}
	}

	void AssetRegistry::UnloadAsset(const std::filesystem::path& path)
	{

	}

	AssetType AssetRegistry::GetAssetTypeFromExtension(const std::string& extension)
	{
		if (extension == ".slang" || extension == ".shader")
			return AssetType::Shader;
		else if (extension == ".mat" || extension == ".material")
			return AssetType::Material;
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
		}

		EC_CORE_ERROR("Unknown asset type for: {0}", metadata.Path.string());
		return nullptr;
	}

}