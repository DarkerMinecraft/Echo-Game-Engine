#pragma once

#include <ostream>
#include <fstream>

namespace Echo 
{

	class IBinarySerializer 
	{
	public:
		virtual uint32_t GetVersion() = 0;

		virtual bool Serialize(std::ostream& stream) = 0;
		virtual bool Deserialize(std::ifstream& stream) = 0;
	};

}