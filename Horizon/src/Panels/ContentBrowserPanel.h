#pragma once

#include <filesystem>
#include <Graphics/Texture.h>

namespace Echo 
{

	class ContentBrowserPanel 
	{
	public:
		ContentBrowserPanel();

		void OnImGuiRender();
	private:
		std::filesystem::path m_CurrentDirectory;

		Ref<Texture2D> m_DirectoryIcon, m_FileIcon;
	};

}