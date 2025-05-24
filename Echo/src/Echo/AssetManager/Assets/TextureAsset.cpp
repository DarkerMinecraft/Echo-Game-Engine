#include "pch.h"
#include "TextureAsset.h"

#include "Graphics/RHISpecification.h"

namespace Echo
{

	TextureAsset::~TextureAsset()
	{
		Destroy();
	}

	void TextureAsset::Load()
	{
		Texture2DSpecification spec{};
		spec.MinFilter = (TextureFilter)(std::get<int>(m_Metadata.CustomProps["MinFilter"]));
		spec.MagFilter = (TextureFilter)(std::get<int>(m_Metadata.CustomProps["MagFilter"]));

		m_Texture = Texture2D::Create(m_Metadata.Path, spec);
		m_Loaded = true;
	}

	void TextureAsset::Reload()
	{

	}

	void TextureAsset::Destroy()
	{
		if (!m_Loaded) return;

		m_Texture->Destroy();
		m_Loaded = false;
	}

	bool TextureAsset::CheckForChanges()
	{
		return false;
	}

}

