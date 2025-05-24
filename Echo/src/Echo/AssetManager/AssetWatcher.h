#pragma once

#include "AssetRegistry.h"

#include <thread>

namespace Echo 
{

	class AssetWatcher 
	{
	public:
		AssetWatcher();
		~AssetWatcher();
	private:
		void Start();
		void WatcherThread();
		void End();
	private:
		std::thread m_AssetWatcherThread;
		bool m_Running = false;
	};

}