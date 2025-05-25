#pragma once

#include "Graphics/Texture.h"

#include <imgui.h>
#include <unordered_map>

namespace Echo 
{

	class ImGuiTextureRegistry 
	{
	public:
		static ImTextureID RegisterTexture(Texture2D* texture);
		static void UnregisterTexture(ImTextureID textureID);

		static Texture2D* GetTexture(ImTextureID textureID);

		static void Clear();
	private:
		static std::unordered_map<ImTextureID, Texture2D*> s_TextureMap;
		static ImTextureID s_NextID;
	};

}