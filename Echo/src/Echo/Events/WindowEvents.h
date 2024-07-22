#pragma once

#include "Event.h"
#include <sstream>

namespace Echo
{

	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		virtual EventType GetType() const override { return EventType::WindowClose; }
		virtual std::string ToString() const override { return "WindowCloseEvent"; }
	};

	class WindowResizeEvent : public Event 
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width), m_Height(height) {}

		virtual EventType GetType() const override { return EventType::WindowResize; }
		virtual std::string ToString() const override 
		{
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }
	private:
		unsigned int m_Width, m_Height;
	};
}