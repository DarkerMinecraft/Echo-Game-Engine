#pragma once

#include <filesystem>
#include <Graphics/Texture.h>

namespace Echo 
{

	class ContentBrowserPanel 
	{
	public:
		ContentBrowserPanel(const std::filesystem::path currentDirectory);
		ContentBrowserPanel()
			: m_CurrentDirectory("") {}

		void OnImGuiRender();

		void SetCurrentDirectory(const std::filesystem::path currentDirectory) { m_CurrentDirectory = currentDirectory; }
	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon, m_FileIcon;
	};

}