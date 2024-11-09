#pragma once

namespace Echo 
{
	class IDevice 
	{
	public:
		~IDevice() = default;

		virtual void Shutdown() = 0;
	};
}