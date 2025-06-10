#pragma once

#include "Graphics/Primitives/Texture.h"
#include "Graphics/Primitives/Framebuffer.h"

#include <imgui.h>
#include <unordered_map>

namespace Echo 
{

	struct ImGuiFramebufferBinding
	{
		Framebuffer* framebuffer;
		uint32_t attachmentIndex;
	};

	class ImGuiTextureRegistry 
	{
	public:
		static ImTextureID RegisterTexture(Texture2D* texture);
		static ImTextureID RegisterFramebuffer(Framebuffer* framebuffer, uint32_t attachmentIndex);

		static void UnregisterTexture(ImTextureID textureID);

		static ImGuiFramebufferBinding* GetFramebuffer(ImTextureID textureID);
		static Texture2D* GetTexture(ImTextureID textureID);

		static void Clear();
	private:
		static std::unordered_map<ImTextureID, Texture2D*> s_TextureMap;
		static std::unordered_map<ImTextureID, ImGuiFramebufferBinding> s_FramebufferMap;
		static ImTextureID s_NextID;
	};

}