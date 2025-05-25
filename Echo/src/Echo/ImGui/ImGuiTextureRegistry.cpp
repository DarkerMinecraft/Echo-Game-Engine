#include "pch.h"
#include "ImGuiTextureRegistry.h"

namespace Echo 
{
	
	std::unordered_map<ImTextureID, Texture2D*> ImGuiTextureRegistry::s_TextureMap;
	ImTextureID ImGuiTextureRegistry::s_NextID = (ImTextureID)1;

	ImTextureID ImGuiTextureRegistry::RegisterTexture(Texture2D* texture)
	{
		if (!texture)
		{
			return 0; 
		}

		ImTextureID id = s_NextID;
		s_TextureMap[id] = texture;
		s_NextID = (ImTextureID)((uintptr_t)s_NextID + 1); 

		return id;
	}

	void ImGuiTextureRegistry::UnregisterTexture(ImTextureID textureID)
	{
		if (textureID == 0) return;

		auto it = s_TextureMap.find(textureID);
		if (it != s_TextureMap.end())
		{
			s_TextureMap.erase(it);
		}
	}

	Texture2D* ImGuiTextureRegistry::GetTexture(ImTextureID textureID)
	{
		if (textureID == 0) return nullptr;

		auto it = s_TextureMap.find(textureID);
		if (it != s_TextureMap.end())
		{
			return it->second;
		}

		EC_CORE_WARN("ImGui texture ID not found: {0}", (void*)textureID);
		return nullptr;
	}

	void ImGuiTextureRegistry::Clear()
	{
		s_TextureMap.clear();
		s_NextID = (ImTextureID)1;
	}

}