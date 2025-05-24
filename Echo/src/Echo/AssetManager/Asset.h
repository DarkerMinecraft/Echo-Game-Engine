#pragma once

#include "AssetMetadata.h"

namespace Echo 
{

	class Asset 
	{
	public:
		virtual ~Asset() = default; 

		virtual void Load() = 0;
		virtual void Reload() = 0;
		virtual void Destroy() = 0; 
		
		virtual bool IsLoaded() = 0;

		virtual bool CheckForChanges() = 0;
		
		virtual AssetMetadata GetMetadata() = 0;
		virtual const AssetMetadata GetMetadata() const = 0;
	};

}