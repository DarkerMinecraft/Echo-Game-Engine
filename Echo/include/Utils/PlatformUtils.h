#pragma once 

#include <string>

namespace Echo 
{

	class FileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};

}