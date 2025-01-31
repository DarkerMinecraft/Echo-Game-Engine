#pragma once

namespace Echo 
{

	class Buffer 
	{
	public:
		virtual ~Buffer() = default;

		virtual void BindBuffer() = 0;
		virtual void WriteToBuffer(void* data) = 0;

		virtual void* GetNativeBuffer() = 0;
	};
	
}