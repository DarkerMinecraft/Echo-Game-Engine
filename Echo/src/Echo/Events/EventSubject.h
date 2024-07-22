#pragma once

#include "Observer.h"
#include "Event.h"

#include "pch.h"

namespace Echo
{

	class EventSubject
	{
	public:

		~EventSubject() 
		{
			m_Observers.clear();
		}

		static EventSubject* Get() 
		{
			if (!s_Instance)
				s_Instance = new EventSubject();
			return s_Instance;
		}

		void Attach(Observer* observer) 
		{
			m_Observers.push_back(observer);
		};

		void Detach(Observer* observer) 
		{
			m_Observers.erase(std::remove(m_Observers.begin(), m_Observers.end(), observer), m_Observers.end());
		};

		void Notify(const Event& e)
		{
			for (Observer* observer : m_Observers)
			{
				observer->OnNotify(e);
			}
		};
	private:
		static EventSubject* s_Instance;
		std::vector<Observer*> m_Observers;
	};

}