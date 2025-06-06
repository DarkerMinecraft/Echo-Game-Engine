#pragma once 

#include <xhash>

namespace Echo 
{

	class UUID 
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;
		
		operator uint64_t() const { return m_UUID; }
		bool operator==(const UUID& other) const { return m_UUID == other.m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

namespace std 
{
	template<>
	struct hash<Echo::UUID> 
	{
		std::size_t operator()(const Echo::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};
}