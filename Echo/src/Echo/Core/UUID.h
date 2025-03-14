#pragma once 

#include <uuid_v4.h>
#include <endianness.h>

namespace Echo 
{

	class UUID
	{
	public:
		UUID() : m_UUID(s_UUIDGenerator.getUUID()) {}

		bool operator==(const UUID& other) const
		{
			return m_UUID == other.m_UUID;
		}
	private:
		UUIDv4::UUID m_UUID;

		static UUIDv4::UUIDGenerator<std::mt19937_64> s_UUIDGenerator;
	};
}