#include <Echo.h>

#include "Echo/Core/Log.h"
#include "Echo/Events/WindowEvents.h"
#include "Echo/Events/EventSubject.h"
#include "Echo/Events/Observer.h"

namespace Echo
{

	class Editor : public Application 
	{
	public: 
		Editor() 
		{
			EventSubject::Get()->Attach(this);
		}
		virtual ~Editor() {}
		
		virtual void OnNotify(const Event& e) override 
		{

		}
	};

	Application* CreateApplication()
	{
		return new Editor();
	}

}