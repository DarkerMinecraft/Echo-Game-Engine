#include "pch.h"
#include "ImGuiTextureRegistry.h"

namespace Echo 
{
	
	std::unordered_map<ImTextureID, Texture2D*> ImGuiTextureRegistry::s_TextureMap;
	std::unordered_map<ImTextureID, ImGuiFramebufferBinding> ImGuiTextureRegistry::s_FramebufferMap;

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

	ImTextureID ImGuiTextureRegistry::RegisterFramebuffer(Framebuffer* framebuffer, uint32_t attachmentIndex)
	{
		if (!framebuffer) return 0;

		ImTextureID id = s_NextID;
		s_FramebufferMap[id] = { framebuffer, attachmentIndex };
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

	ImGuiFramebufferBinding* ImGuiTextureRegistry::GetFramebuffer(ImTextureID textureID)
	{
		auto it = s_FramebufferMap.find(textureID);
		return (it != s_FramebufferMap.end()) ? &it->second : nullptr;
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
		s_FramebufferMap.clear();
		s_NextID = (ImTextureID)1;
	}

}