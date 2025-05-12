#pragma once

namespace Echo
{

	enum class AssetType 
	{
		None = 0,
		Texture,
		Mesh, 
		Material,
		Shader,
		Scene,
		Audio,
		Font,
		Script
	};

	class Asset 
	{
	public:
		virtual ~Asset() = default;

		AssetType GetAssetType() {}
	private:
		AssetType m_AssetType;

	};

}