#pragma once

namespace Echo 
{
	class GraphicsContext
	{
	public:
		virtual void Init() = 0;
		virtual void SwapBuffers() = 0;

		virtual void CleanUp() = 0;
	};
}
