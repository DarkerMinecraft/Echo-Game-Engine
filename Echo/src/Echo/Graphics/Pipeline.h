#pragma once

namespace Echo 
{
	
	class Pipeline 
	{
	public:
		virtual ~Pipeline() = default;

		virtual void Bind() = 0;

		virtual void WritePushConstants(const void* pushConstants) = 0;
	};

}