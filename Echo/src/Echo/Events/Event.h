#pragma once

namespace Echo 
{

	enum class EventType 
	{
		None = 0, 
		WindowClose, WindowResize, 
	};

	class Event 
	{
	public:
		virtual ~Event() {}

		virtual EventType GetType() const = 0;
		virtual std::string ToString() const = 0;
	};
}