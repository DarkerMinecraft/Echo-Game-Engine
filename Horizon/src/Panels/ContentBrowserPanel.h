#pragma once

#include <filesystem>
#include <AssetManager/Assets/TextureAsset.h>

namespace Echo 
{

	class ContentBrowserPanel 
	{
	public:
		ContentBrowserPanel(const std::filesystem::path currentDirectory);
		ContentBrowserPanel();

		void OnImGuiRender();

		void SetGlobalDirectory(const std::filesystem::path globalDirectory) { m_GlobalDirectory = globalDirectory; m_CurrentDirectory = globalDirectory; }
	private:
		std::filesystem::path m_CurrentDirectory;
		std::filesystem::path m_GlobalDirectory;

		Ref<TextureAsset> m_DirectoryIcon, m_FileIcon;
	};

}