#include "pch.h"
#include "AssetWatcher.h"

namespace Echo 
{
	
	AssetWatcher::AssetWatcher()
	{
		Start();
	}

	AssetWatcher::~AssetWatcher()
	{
		End();
	}

	void AssetWatcher::Start()
	{
		m_Running = true;
		m_AssetWatcherThread = std::thread(&AssetWatcher::WatcherThread, this);
	}

	void AssetWatcher::WatcherThread()
	{
		while (m_Running) 
		{
			std::vector<Ref<Asset>> assets = AssetRegistry::GetAllLoadedAssets();
			for (auto& asset : assets) 
			{
				if (!asset || !asset.get())
				{
					continue; 
				}

				bool hasChanged = asset->CheckForChanges();
				if (hasChanged) 
				{
					asset->Reload();
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	}

	void AssetWatcher::End()
	{
		m_Running = false;
		m_AssetWatcherThread.join();
	}

}