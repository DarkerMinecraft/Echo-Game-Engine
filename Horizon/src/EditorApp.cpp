#include "EditorLayer.h"

#include <Echo/Core/EntryPoint.h>
#include <Echo/Core/Application.h>

namespace Echo
{

	class Editor : public Application 
	{
	public: 
		Editor()
			: Application("Horizon")
		{
			PushLayer(new EditorLayer());
		}
		~Editor() 
		{
	
		}
	};

	Application* CreateApplication()
	{
		return new Editor();
	}

}