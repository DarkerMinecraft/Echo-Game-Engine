#pragma once

#include "AssetMetadata.h"

namespace Echo 
{

	class Asset 
	{
	public:
		virtual ~Asset(); 

		virtual void Load() = 0;
		virtual void Reload() = 0;
		virtual void Destroy() = 0; 
		
		AssetMetadata GetMetadata() { return m_Metadata; }
		const AssetMetadata GetMetadata() const { return m_Metadata; }
		
		bool IsDirty() { return m_IsDirty; }
		bool IsLoaded() { return m_IsLoaded; }
	private:
		AssetMetadata m_Metadata;

		bool m_IsDirty;
		bool m_IsLoaded;
	};

}