#include "pch.h"
#include "UUID.h"

namespace Echo 
{

	UUIDv4::UUIDGenerator<std::mt19937_64> UUID::s_UUIDGenerator = UUIDv4::UUIDGenerator<std::mt19937_64>();

}