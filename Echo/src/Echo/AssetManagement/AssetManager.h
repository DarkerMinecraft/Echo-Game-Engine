#pragma once 

#include <string>

namespace Echo 
{

	class AssetManager 
	{
	public:
		AssetManager(const std::string& assetPath)
			: m_AssetPath(assetPath) {}

		~AssetManager();
	private:
		std::string m_AssetPath;
	};

}