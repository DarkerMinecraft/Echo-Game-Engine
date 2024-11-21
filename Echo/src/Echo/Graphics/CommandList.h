#pragma once

namespace Echo 
{

	class CommandPool 
	{
	public:
		virtual ~CommandPool() = default;

		virtual void* GetPool() = 0;
	};

	class CommandBuffer 
	{
	public: 
		virtual ~CommandBuffer() = default;

		virtual void* GetBuffer() = 0;

		virtual void Begin() = 0;
		virtual void End() = 0;
	};

}