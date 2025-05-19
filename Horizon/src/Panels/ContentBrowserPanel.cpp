#include "ContentBrowserPanel.h"

#include <Debug/Instrumentor.h>

#include <imgui.h>
#include <Core/Log.h>

namespace Echo 
{
	
	ContentBrowserPanel::ContentBrowserPanel(const std::filesystem::path currentDirectory)
		: m_CurrentDirectory(currentDirectory)
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	ContentBrowserPanel::ContentBrowserPanel()
		: m_CurrentDirectory("")
	{
		m_DirectoryIcon = Texture2D::Create("Resources/Icons/ContentBrowser/DirectoryIcon.png");
		m_FileIcon = Texture2D::Create("Resources/Icons/ContentBrowser/FileIcon.png");
	}

	void ContentBrowserPanel::OnImGuiRender()
	{
		EC_PROFILE_FUNCTION();
		ImGui::Begin("Content Browser");
		
		if (m_CurrentDirectory != std::filesystem::path(m_CurrentDirectory))
		{
			if (ImGui::Button("<-"))
			{
				m_CurrentDirectory = m_CurrentDirectory.parent_path();
			}
		}

		static float padding = 16.0f;
		static float thumbnailSize = 60.0f;
		float cellSize = thumbnailSize + padding;

		float panelWidth = ImGui::GetContentRegionAvail().x;
		int columnCount = (int)(panelWidth / cellSize);
		if (columnCount < 1)
			columnCount = 1;

		ImGui::Columns(columnCount, 0, false);

		for (auto& directoryEntry : std::filesystem::directory_iterator(m_CurrentDirectory)) 
		{
			const auto& path = directoryEntry.path();
			std::string pathName = directoryEntry.path().string();
			auto relativePath = std::filesystem::relative(path, m_CurrentDirectory);
			std::string filenameString = relativePath.filename().string();

			ImGui::PushID(filenameString.c_str());
			Ref<Texture2D> icon = directoryEntry.is_directory() ? m_DirectoryIcon : m_FileIcon;
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
			ImGui::ImageButton(filenameString.c_str(), (ImTextureID)icon->GetResourceID(), { thumbnailSize, thumbnailSize });

			if (ImGui::BeginDragDropSource()) 
			{
				const wchar_t* itemPath = path.c_str();

				ImGui::SetDragDropPayload("CONTENT_BROWSER_ITEM", itemPath, (wcslen(itemPath) + 1) * sizeof(wchar_t));
				ImGui::EndDragDropSource();
			}

			ImGui::PopStyleColor();
			if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) 
			{
				if (directoryEntry.is_directory())
				{
					m_CurrentDirectory /= path.filename();
				}
			}
			ImGui::TextWrapped(relativePath.stem().string().c_str());

			ImGui::NextColumn();

			ImGui::PopID();
		}
		ImGui::Columns(1);

		ImGui::End();
	}

}