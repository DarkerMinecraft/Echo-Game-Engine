#pragma once

#include "Event.h"

namespace Echo 
{

	class Observer 
	{
	public:
		virtual ~Observer() {}
		virtual void OnNotify(const Event& e) = 0;
	};

}