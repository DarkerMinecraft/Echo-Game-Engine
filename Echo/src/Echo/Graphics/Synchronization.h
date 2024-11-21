#pragma once

namespace Echo 
{

	class Fence 
	{
	public:
		virtual ~Fence() = default;

		virtual void* GetFence() = 0;

		virtual void Wait() = 0;
		virtual void Reset() = 0;
	};
	
	class Semaphore 
	{
	public:
		virtual ~Semaphore() = default;

		virtual void* GetSemaphore() = 0;
	};

}