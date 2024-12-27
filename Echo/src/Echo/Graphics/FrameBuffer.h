#pragma once

namespace Echo 
{

	class FrameBuffer 
	{
	public:
		virtual ~FrameBuffer() = default;

		virtual void Start() = 0;
		virtual void End() = 0;
	};

}